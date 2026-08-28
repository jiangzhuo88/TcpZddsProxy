#ifndef VERSION_H
#define VERSION_H

#include <QString>

namespace AppVersion {
    static const QString Version = "v1.0";
    static const QString BuildDate = "2026-08-11";
    static const QString Notes =
        "v1.0 - Initial Release\n"
        "==============================\n"
        "1. TCP-ZDDS bidirectional proxy (Server/Client mode)\n"
        "2. Auto-reconnect with configurable interval\n"
        "3. Multi-language support (Chinese/English)\n"
        "4. Configuration persistence (config.json)\n"
        "5. Message code (unMsgCode) support with stDMHead\n"
        "6. Single-process mode\n"
        "7. Data flow statistics and status indicators\n"
        "\n"
        "Fixes:\n"
        "- Cross-thread ZDDS callback safety (QMetaObject::invokeMethod)\n"
        "- Translation loading order fix (retranslateUi after language switch)\n"
        "- TCP disconnect button state fix\n"
        "- Reconnect interval label translation fix";
}

#endif // VERSION_H
