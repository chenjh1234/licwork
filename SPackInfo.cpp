#include "SPackInfo.h"

SPackInfo::SPackInfo():LInfo()
{
    init();
}
SPackInfo::~SPackInfo()
{

}

void SPackInfo::init()
{
    limit = 0;
    used = 0;
}
