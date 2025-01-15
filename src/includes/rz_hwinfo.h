#pragma once

#include <hwinfo/hwinfo.h>
#include <hwinfo/utils/unit.h>

#include <QMap>

using hwinfo::unit::bytes_to_MiB;

class HwInfo
{
public:
    HwInfo();

    void setHwInfo();
    QMap<QString, QString> getHwInfo();

private:
    QMap<QString, QString> hw_info;
};
