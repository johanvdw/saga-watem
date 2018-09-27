#ifndef SEGMENT_CATCHMENT_H
#define SEGMENT_CATCHMENT_H


#include "TLB_Interface.h"

class segment_catchment : public CSG_Tool_Grid
{
public:
    segment_catchment();
    virtual ~segment_catchment(void);

protected:
    virtual bool			On_Execute(void);

private:
    sLong pos(const int row, const int col);
};

#endif // SEGMENT_CATCHMENT_H
