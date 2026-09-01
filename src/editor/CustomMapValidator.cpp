#include "CustomMapValidator.h"

CustomMapValidator::Result CustomMapValidator::validate(const CustomMapData& data) {
    Result r;

    const int spawns  = data.countPlayerSpawns();
    const int bosses  = data.countBosses();

    // --- Thể loại A: 1 người chơi + đúng 1 boss ---------------------------
    if (spawns == 1 && bosses == 1) {
        r.valid   = true;
        r.mode    = PlayMode::SinglePlusBoss;
        r.message = "Che do 1 nguoi choi dau boss.";
        return r;
    }

    // --- Thể loại B: đối kháng 2 người, không boss -------------------------
    if (spawns == 2 && bosses == 0) {
        r.valid   = true;
        r.mode    = PlayMode::Versus;
        r.message = "Che do doi khang 2 nguoi.";
        return r;
    }

    // --- Không hợp lệ: nêu rõ vi phạm -------------------------------------
    // Thông báo phải nói được người chơi đang SAI Ở ĐÂU và CẦN LÀM GÌ, chứ
    // không chỉ "map khong hop le".
    r.valid = false;
    r.mode  = PlayMode::Invalid;

    if (spawns == 0) {
        r.message = "Map chua co diem xuat phat nao. Can 1 PlayerSpawn + 1 Boss, "
                    "hoac 2 PlayerSpawn.";
    } else if (spawns > 2) {
        r.message = "Co " + std::to_string(spawns) + " PlayerSpawn. Chi cho phep 1 "
                    "(kem 1 boss) hoac 2 (doi khang).";
    } else if (spawns == 1 && bosses == 0) {
        r.message = "Map 1 nguoi choi phai co dung 1 Boss. Hien tai chua dat boss nao.";
    } else if (spawns == 1 && bosses > 1) {
        r.message = "Map 1 nguoi choi chi duoc co 1 Boss, hien co "
                    + std::to_string(bosses) + ".";
    } else if (spawns == 2 && bosses > 0) {
        r.message = "Map doi khang 2 nguoi khong duoc co Boss, hien co "
                    + std::to_string(bosses) + ".";
    } else {
        r.message = "Cau hinh khong hop le: " + std::to_string(spawns)
                  + " PlayerSpawn, " + std::to_string(bosses) + " Boss.";
    }

    return r;
}
