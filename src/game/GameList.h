#ifndef GAME_LIST_H_
#define GAME_LIST_H_

#include <vector>
#include <stdint.h>
#include <coreinit/mcp.h>
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
    GameList() : selectedGame(0) { };
    ~GameList() { clear(); };

    int32_t size() const {
        return filteredList.size();
    }
    int32_t gameCount() const {
        return fullGameList.size();
    }
    int32_t filterList(const char * gameFilter = NULL);
    int32_t loadUnfiltered();

    gameInfo * at(int32_t i) const {
        return operator[](i);
    }
    gameInfo * operator[](int32_t i) const {
        if (i < 0 || i >= (int32_t) filteredList.size())
            return NULL;
        return filteredList[i];
    }
    gameInfo * getGameInfo(uint64_t titleId) const;

    const char * getCurrentFilter() const {
        return gameFilter.c_str();
    }
    void sortList();
    void clear();
    bool operator!() const {
        return (fullGameList.size() == 0);
    }

    //! Gamelist scrolling operators
    int32_t operator+=(int32_t i) {
        return (selectedGame = (selectedGame+i) % filteredList.size());
    }
    int32_t operator-=(int32_t i) {
        return (selectedGame = (selectedGame-i+filteredList.size()) % filteredList.size());
    }
    int32_t operator++() {
        return (selectedGame = (selectedGame+1) % filteredList.size());
    }
    int32_t operator--() {
        return (selectedGame = (selectedGame-1+filteredList.size()) % filteredList.size());
    }
    int32_t operator++(int32_t i) {
        return operator++();
    }
    int32_t operator--(int32_t i) {
        return operator--();
    }
    gameInfo * GetCurrentSelected() const {
        return operator[](selectedGame);
    }

    std::vector<gameInfo *> & getfilteredList(void) {
        return filteredList;
    }
    std::vector<gameInfo *> & getFullGameList(void) {
        return fullGameList;
    }

    sigslot::signal1<GameList *> titleListChanged;
    sigslot::signal1<gameInfo *> titleUpdated;
    sigslot::signal1<gameInfo *> titleAdded;
protected:

    int32_t readGameList();

    void internalFilterList(std::vector<gameInfo*> & fullList);
    void internalLoadUnfiltered(std::vector<gameInfo*> & fullList);

    void loadIcons();

    static bool nameSortCallback(const gameInfo *a, const gameInfo *b);

    static GameList *gameListInstance;

    std::string gameFilter;
    int32_t selectedGame;
    std::vector<gameInfo *> filteredList;
    std::vector<gameInfo *> fullGameList;
};

#endif
