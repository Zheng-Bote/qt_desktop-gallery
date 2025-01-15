#include "rz_hwinfo.h"

using hwinfo::unit::bytes_to_MiB;

HwInfo::HwInfo() {}

void HwInfo::setHwInfo()
{
    const auto cpus = hwinfo::getAllCPUs();
    for (const auto &cpu : cpus) {
        hw_info.insert("CPU vendor", cpu.vendor().c_str());
        hw_info.insert("CPU model", cpu.modelName().c_str());
        hw_info.insert("CPU physical cores", QString::number(cpu.numPhysicalCores()));
        hw_info.insert("CPU logical cores", QString::number(cpu.numLogicalCores()));
        hw_info.insert("CPU max frequency", QString::number(cpu.maxClockSpeed_MHz()));
    }

    hwinfo::Memory memory;
    hw_info.insert("RAM size [MiB]", QString::number(bytes_to_MiB(memory.total_Bytes())));
    hw_info.insert("RAM free [MiB]", QString::number(bytes_to_MiB(memory.free_Bytes())));
    hw_info.insert("RAM available [MiB]", QString::number(bytes_to_MiB(memory.available_Bytes())));

    hwinfo::OS os;
    hw_info.insert("Operating System", os.name().c_str());
    hw_info.insert("OS version", os.version().c_str());
    hw_info.insert("OS kernel", os.kernel().c_str());
    hw_info.insert("OS architecture", os.is32bit() ? "32 bit" : "64 bit");
}

QMap<QString, QString> HwInfo::getHwInfo()
{
    return hw_info;
}
