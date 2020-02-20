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
    ~GameList() {
        clear();
    };

    int32_t size() {
        lock();
        int32_t res = filteredList.size();
        unlock();
        return res;
    }
    int32_t gameCount() {
        lock();
        int32_t res = fullGameList.size();
        unlock();
        return res;
    }
    int32_t filterList(const char * gameFilter = NULL);
    int32_t loadUnfiltered();

    gameInfo * at(int32_t i) {
        return operator[](i);
    }
    gameInfo * operator[](int32_t i) {
        lock();
        gameInfo * res = NULL;
        if (i < 0 || i >= (int32_t) filteredList.size())
            res = NULL;
        res =  filteredList[i];
        unlock();
        return res;
    }
    gameInfo * getGameInfo(uint64_t titleId) const;

    const char * getCurrentFilter() const {
        return gameFilter.c_str();
    }
    void sortList();
    void clear();
    bool operator!() {
        lock();
        bool res = (fullGameList.size() == 0);
        unlock();
        return res;
    }

    //! Gamelist scrolling operators
    int32_t operator+=(int32_t i) {
        lock();
        int32_t res = (selectedGame = (selectedGame+i) % filteredList.size());
        unlock();
        return res;
    }
    int32_t operator-=(int32_t i) {
        lock();
        int32_t res = (selectedGame = (selectedGame-i+filteredList.size()) % filteredList.size());
        unlock();
        return res;
    }
    int32_t operator++() {
        lock();
        int32_t res =  (selectedGame = (selectedGame+1) % filteredList.size());
        unlock();
        return res;
    }
    int32_t operator--() {
        lock();
        int32_t res =  (selectedGame = (selectedGame-1+filteredList.size()) % filteredList.size());
        unlock();
        return res;
    }
    int32_t operator++(int32_t i) {
        return operator++();
    }
    int32_t operator--(int32_t i) {
        return operator--();
    }
    gameInfo * GetCurrentSelected() {
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

    void lock() {
        _lock.lock();
    }
    void unlock(){
        _lock.unlock();
    }
protected:

    int32_t readGameList();

    void internalFilterList(std::vector<gameInfo*> & fullList);
    void internalLoadUnfiltered(std::vector<gameInfo*> & fullList);

    void updateTitleInfo();

    static bool nameSortCallback(const gameInfo *a, const gameInfo *b);

    static GameList *gameListInstance;

    std::string gameFilter;
    int32_t selectedGame;
    std::vector<gameInfo *> filteredList;
    std::vector<gameInfo *> fullGameList;

    CMutex _lock;
};

#endif
