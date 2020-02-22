#include <algorithm>
#include <string>
#include <string.h>
#include <coreinit/mcp.h>
#include <nn/acp/nn_acp_types.h>
#include <coreinit/cache.h>

#include "utils/AsyncExecutor.h"
#include "GameList.h"
#include "common/common.h"

#include "fs/DirList.h"
#include "fs/FSUtils.h"
#include "utils/logger.h"
#include "utils/StringTools.h"

void GameList::clear() {
    lock();
    for (auto const& x : fullGameList) {
        if(x != NULL) {
            if(x->imageData != NULL) {
                AsyncExecutor::pushForDelete(x->imageData);
                x->imageData = NULL;
            }
            delete x;
        }
    }
    gameFilter.clear();
    fullGameList.clear();
    filteredList.clear();
    //! Clear memory of the vector completely
    std::vector<gameInfo *>().swap(filteredList);
    std::vector<gameInfo*>().swap(fullGameList);

    unlock();
    titleListChanged(this);
}

gameInfo * GameList::getGameInfo(uint64_t titleId) const {
    for (uint32_t i = 0; i < filteredList.size(); ++i) {
        if(titleId == filteredList[i]->titleId)
            return filteredList[i];
    }

    return NULL;
}

extern "C" int ACPGetTitleMetaXml(uint64_t titleid, ACPMetaXml*);
int32_t GameList::readGameList() {
    // Clear list
    for (auto const& x : fullGameList) {
        delete x;
    }

    fullGameList.clear();
    //! Clear memory of the vector completely
    std::vector<gameInfo*>().swap(fullGameList);

    int32_t cnt = 0;

    MCPError mcp = MCP_Open();
    if (mcp < 0) {
        return 0;
    }

    MCPError titleCount = MCP_TitleCount(mcp);
    if (titleCount < 0) {
        MCP_Close(mcp);
        return 0;
    }

    std::vector<struct MCPTitleListType> titles(titleCount);
    uint32_t realTitleCount;

    MCPError err = MCP_TitleList(mcp, &realTitleCount, titles.data(), titles.size() * sizeof(decltype(titles)::value_type));
    if (err < 0) {
        MCP_Close(mcp);
        return 0;
    }
    if (realTitleCount != titles.size()) {
        titles.resize(realTitleCount);
    }

    for (auto title_candidate : titles) {
        if((title_candidate.titleId & 0xFFFFFFFF00000000L) == 0x0005000000000000L) {
            gameInfo* newGameInfo = new gameInfo;
            newGameInfo->titleId = title_candidate.titleId;
            newGameInfo->gamePath = title_candidate.path;
            newGameInfo->name = "<unknown>";
            newGameInfo->imageData = NULL;
            DCFlushRange(newGameInfo, sizeof(gameInfo));

            fullGameList.push_back(newGameInfo);
            titleAdded(newGameInfo);
            cnt++;
        }
    }

    AsyncExecutor::execute([this] {
        lock();
        for (uint32_t i = 0; i < fullGameList.size(); ++i) {
            gameInfo *header = fullGameList[i];

            DCFlushRange(&stopAsyncLoading, sizeof(stopAsyncLoading));
            if(stopAsyncLoading) {
                DEBUG_FUNCTION_LINE("Stop async title loading\n");
                break;
            }

            DEBUG_FUNCTION_LINE("Load extra infos of %016llX\n",header->titleId);
            ACPMetaXml* meta = (ACPMetaXml*)calloc(1, 0x4000); //TODO fix wut
            if(meta) {
                auto acp = ACPGetTitleMetaXml(header->titleId, meta);
                if(acp >= 0) {
                    header->name = meta->shortname_en;
                }
                free(meta);
            }

            if(header->imageData == NULL) {
                std::string filepath = "fs:" + header->gamePath + META_PATH + "/iconTex.tga";
                uint8_t *buffer = NULL;
                uint32_t bufferSize = 0;
                int iResult = FSUtils::LoadFileToMem(filepath.c_str(), &buffer, &bufferSize);
                if(iResult > 0) {
                    GuiImageData * imageData = new GuiImageData(buffer, bufferSize, GX2_TEX_CLAMP_MODE_MIRROR);
                    if(imageData) {
                        header->imageData = imageData;
                    }

                    //! free original image buffer which is converted to texture now and not needed anymore
                    free(buffer);
                }
            }
            DCFlushRange(header, sizeof(gameInfo));
            titleUpdated(header);
        }
        unlock();
    });

    return cnt;
}

void GameList::updateTitleInfo() {
    for (int i = 0; i < this->size(); i++) {
        gameInfo * newHeader = this->at(i);

        bool hasChanged = false;

        if(newHeader->name.empty()) {
            ACPMetaXml* meta = (ACPMetaXml*)calloc(1, 0x4000); //TODO fix wut
            if(meta) {
                auto acp = ACPGetTitleMetaXml(newHeader->titleId, meta);
                if(acp >= 0) {
                    newHeader->name = meta->shortname_en;
                    hasChanged = true;
                }
                free(meta);
            }
        }

        if(newHeader->imageData == NULL) {
            std::string filepath = "fs:" + newHeader->gamePath + META_PATH + "/iconTex.tga";
            uint8_t *buffer = NULL;
            uint32_t bufferSize = 0;
            int iResult = FSUtils::LoadFileToMem(filepath.c_str(), &buffer, &bufferSize);

            if(iResult > 0) {
                GuiImageData * imageData = new GuiImageData(buffer, bufferSize, GX2_TEX_CLAMP_MODE_MIRROR);
                if(imageData) {
                    newHeader->imageData = imageData;
                    hasChanged = true;
                }

                //! free original image buffer which is converted to texture now and not needed anymore
                free(buffer);
            }
        }
        if(hasChanged) {
            DCFlushRange(newHeader, sizeof(gameInfo));
            titleUpdated(newHeader);
        }
    }
}

void GameList::internalFilterList(std::vector<gameInfo*> &fullList) {
    for (uint32_t i = 0; i < fullList.size(); ++i) {
        gameInfo *header = fullList[i];
        if (StringTools::findStringIC(header->name,gameFilter)) {
            filteredList.push_back(header);
        }
    }
}

int32_t GameList::filterList(const char * filter) {
    lock();
    if(filter) {
        gameFilter = filter;
    }

    if(fullGameList.size() == 0) {
        readGameList();
    }

    filteredList.clear();

    // Filter current game list if selected
    internalFilterList(fullGameList);

    sortList();

    titleListChanged(this);

    AsyncExecutor::execute([&] { updateTitleInfo();});
    int32_t res = filteredList.size();
    unlock();
    return res;
}

void GameList::internalLoadUnfiltered(std::vector<gameInfo*> & fullList) {
    for (uint32_t i = 0; i < fullList.size(); ++i) {
        gameInfo *header = fullList[i];

        filteredList.push_back(header);
    }
}

int32_t GameList::loadUnfiltered() {
    lock();
    if(fullGameList.size() == 0) {
        readGameList();
    }

    gameFilter.clear();
    filteredList.clear();

    // Filter current game list if selected
    internalLoadUnfiltered(fullGameList);

    sortList();

    AsyncExecutor::execute([&] { updateTitleInfo();});

    titleListChanged(this);

    int res = filteredList.size();
    unlock();
    return res;
}

void GameList::sortList() {
    lock();
    std::sort(filteredList.begin(), filteredList.end(), nameSortCallback);
    unlock();
}

bool GameList::nameSortCallback(const gameInfo *a, const gameInfo *b) {
    return (strcasecmp(((gameInfo *) a)->name.c_str(), ((gameInfo *) b)->name.c_str()) < 0);
}

