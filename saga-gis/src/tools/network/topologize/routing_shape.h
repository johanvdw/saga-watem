#ifndef ROUTING_SHAPE_H
#define ROUTING_SHAPE_H

#include "TLB_Interface.h"


class routing_shape : public CSG_Tool
{
public:
    routing_shape(void);
    virtual ~routing_shape(void);
    TSG_Point to_world(int col, int row);


protected:
    virtual bool			On_Execute(void);

private:
    CSG_Grid_System system;
    CSG_Shapes * lines;
    CSG_Grid * landuse;
    void Add_Line(int col, int row, int t_col, int t_row, double dist, double part);
};


enum  class t_f{ // target fields
    col=0,
    row,
    target1col
};

#endif // ROUTING_SHAPE_H
