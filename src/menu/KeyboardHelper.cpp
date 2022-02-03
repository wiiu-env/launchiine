#include "KeyboardHelper.h"
#include "utils/logger.h"
#include <coreinit/memdefaultheap.h>
#include <nn/swkbd.h>

KeyboardHelper::KeyboardHelper() {
    auto *_fsClient = (FSClient *) MEMAllocFromDefaultHeap(sizeof(FSClient));
    FSAddClient(_fsClient, FS_ERROR_FLAG_ALL);

    // Create swkbd
    nn::swkbd::CreateArg createArg;
    createArg.regionType = nn::swkbd::RegionType::Europe;
    createArg.workMemory = MEMAllocFromDefaultHeap(nn::swkbd::GetWorkMemorySize(0));
    memset(createArg.workMemory, 0, sizeof(nn::swkbd::GetWorkMemorySize(0)));
    this->workMemory   = createArg.workMemory;
    createArg.fsClient = _fsClient;
    this->fsClient     = createArg.fsClient;
    DEBUG_FUNCTION_LINE("Calling create");
    if (!nn::swkbd::Create(createArg)) {
        DEBUG_FUNCTION_LINE("Failed to create keyboard");
        return;
    }

    keyboardCreated = true;
}

KeyboardHelper::~KeyboardHelper() {
    if (keyboardCreated) {
        nn::swkbd::Destroy();
        MEMFreeToDefaultHeap(this->workMemory);
        this->workMemory = nullptr;

        FSDelClient(fsClient, FS_ERROR_FLAG_ALL);
        MEMFreeToDefaultHeap(this->fsClient);
        keyboardCreated = false;
    }
}

bool KeyboardHelper::openKeyboard() {
    if (keyboardCreated) {
        // Show the keyboard
        nn::swkbd::AppearArg appearArg;
        appearArg.keyboardArg.configArg.languageType = nn::swkbd::LanguageType::English;
        if (!nn::swkbd::AppearInputForm(appearArg)) {
            DEBUG_FUNCTION_LINE("nn::swkbd::AppearInputForm failed");
            return false;
        }
        keyboardOpen = true;
        return true;
    }
    return false;
}

std::string KeyboardHelper::getResult() {
    return resultStr;
}

bool KeyboardHelper::checkResult() {
    if (keyboardCreated) {
        VPADStatus vpadStatus;
        if (keyboardOpen) {
            VPADRead(VPAD_CHAN_0, &vpadStatus, 1, nullptr);
            VPADGetTPCalibratedPoint(VPAD_CHAN_0, &vpadStatus.tpNormal, &vpadStatus.tpNormal);
        }
        // Update keyboard
        nn::swkbd::ControllerInfo controllerInfo;
        controllerInfo.vpad    = &vpadStatus;
        controllerInfo.kpad[0] = nullptr;
        controllerInfo.kpad[1] = nullptr;
        controllerInfo.kpad[2] = nullptr;
        controllerInfo.kpad[3] = nullptr;
        nn::swkbd::Calc(controllerInfo);

        if (nn::swkbd::IsNeedCalcSubThreadFont()) {
            nn::swkbd::CalcSubThreadFont();
        }

        if (nn::swkbd::IsNeedCalcSubThreadPredict()) {
            nn::swkbd::CalcSubThreadPredict();
        }

        if (nn::swkbd::IsDecideOkButton(nullptr) || nn::swkbd::IsDecideCancelButton(nullptr)) {
            const char16_t *str = nn::swkbd::GetInputFormString();
            // Quick hack to get from a char16_t str to char for our log function
            char logStr[128];
            logStr[0] = 0;

            for (int i = 0; i < 128; ++i) {
                if (!str[i]) {
                    logStr[i] = 0;
                    break;
                }

                if (str[i] > 0x7F) {
                    logStr[i] = '?';
                } else {
                    logStr[i] = str[i];
                }
            }
            this->resultStr = logStr;
            keyboardOpen    = false;
            nn::swkbd::DisappearInputForm();
            return true;
        }
    }
    return false;
}

void KeyboardHelper::drawDRC() {
    nn::swkbd::DrawDRC();
}

void KeyboardHelper::drawTV() {
    nn::swkbd::DrawTV();
}
