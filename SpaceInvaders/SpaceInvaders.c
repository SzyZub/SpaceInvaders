#include "heading.h"
#include <stdio.h>

int main() {
    srand(time(0));
    StrEntStats entStats;
    StrList* enListP = NULL;
    StrList* plBullListP = NULL;
    StrList* enBullListP = NULL;
    StrRect* wallArrayP = NULL;
    StrRect collBox;
    StrFileVars fiVars;
    _initFileVars(&fiVars, 0);
    StrGaState gaState = _initGameState();
    _initAllegroVars();
    while (true) {
        if (enListP == NULL) {
            if (gaState.screenFlag == initiation) {
                gaState.screenFlag = _firstRound(&entStats, &enListP, &plBullListP, &enBullListP, &wallArrayP, &collBox, fiVars, gaState.roundNum);
            } else if (gaState.screenFlag == play) {
                 gaState.screenFlag = _nextRound(&gaState, &fiVars);
            }
        }
        _eventHandler(&(gaState.reDraw), &(gaState.quitProg));
        if (gaState.reDraw && al_is_event_queue_empty(allQueue)) {
            _logic(&gaState, &entStats, &enListP, &plBullListP, &enBullListP, &wallArrayP, &collBox, &fiVars);
        }
        if (gaState.quitProg) {
            exit(0);
        }
    }
}
