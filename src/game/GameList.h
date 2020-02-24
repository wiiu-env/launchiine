#ifndef GAME_LIST_H_
#define GAME_LIST_H_

#include <vector>
#include <mutex>
#include <stdint.h>
#include <coreinit/mcp.h>
#include <coreinit/cache.h>
#include <gui/sigslot.h>
#include <gui/GuiImageData.h>

typedef struct _gameInfo {
    uint64_t titleId;
    std::string name;
    std::string gamePath;
    GuiImageData * imageData;
} gameInfo;

class GameList {
public:
    GameList();
    ~GameList();

    int32_t size() {
        lock();
        int32_t res = fullGameList.size();
        unlock();
        return res;
    }
    int32_t gameCount() {
        lock();
        int32_t res = fullGameList.size();
        unlock();
        return res;
    }

    gameInfo * at(int32_t i) {
        return operator[](i);
    }
    gameInfo * operator[](int32_t i) {
        lock();
        gameInfo * res = NULL;
        if (i < 0 || i >= (int32_t) fullGameList.size()) {
            res = NULL;
        } else {
            res = fullGameList[i];
        }
        unlock();
        return res;
    }
    gameInfo * getGameInfo(uint64_t titleId);

    void clear();

    std::vector<gameInfo *> & getFullGameList(void) {
        return fullGameList;
    }

    int32_t load();

    sigslot::signal1<GameList *> titleListChanged;
    sigslot::signal1<gameInfo *> titleUpdated;
    sigslot::signal1<gameInfo *> titleAdded;

    void lock() {
        _lock.lock();
    }
    void unlock() {
        _lock.unlock();
    }
protected:
    int32_t readGameList();
    void updateTitleInfo();

    std::vector<gameInfo *> fullGameList;

    std::recursive_mutex _lock;

    bool stopAsyncLoading = false;
};

#endif
