#include "stp.h"

void CNetwork::Init()
{
	set_Node.clear();
	set_Link.clear();
	trips.clear();

	memset(&node_Head,0,sizeof(node));
	memset(&node_Tail,0,sizeof(node));
	node_Tail.ln = &node_Head;
	node_Head.rn = &node_Tail;
	node_Head.pv = -1;
	node_Tail.pv = INFI;
	MaxLinkID = 0;
	MaxNodeID = 0;
	m_tolerance = 0;
	m_lock = true;
	delete pSearch;
	pSearch = NULL;
	delete ptmpShpLink;
	ptmpShpLink = NULL;
	delete ptmpShpNode;
	ptmpShpNode = NULL;
}

int CNetwork::Init(CSG_Shapes *pShapes, double tolerance)
{
	ptmpShpNode = SG_Create_Shapes(SHAPE_TYPE_Point);
	ptmpShpLink = SG_Create_Shapes(SHAPE_TYPE_Line);

	if(Topologize(pShapes, ptmpShpNode, ptmpShpLink, tolerance))
	{
		return Init(ptmpShpNode, ptmpShpLink);
	}
	return 0;
}
int CNetwork::Init(CSG_Shapes *pPoints, CSG_Shapes	*pShapes)
{
	if (ValidateVertices(pPoints))
	{
		pSearch = new CSG_Shapes_Search(pPoints);
		if (ValidateTopology(pShapes))
		{
			int		iShape;
			CSG_Shape	*pShape;
			node *pNode;

			set_Node.clear();
			set_Link.clear();
			trips.clear();

			memset(&node_Head,0,sizeof(node));
			memset(&node_Tail,0,sizeof(node));
			node_Tail.ln = &node_Head;
			node_Head.rn = &node_Tail;
			node_Head.pv = -1;
			node_Tail.pv = INFI;
			MaxLinkID = 0;
			MaxNodeID = 0;
			for(iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
			{
				pShape = pShapes->Get_Shape(iShape);
				InsLink(
                    pShape->asInt("ID"),
                    pShape->asInt("O"),
                    pShape->asInt("D"),
                    pShape->asDouble("IMPEDANCE"),
                    pShape->asInt("DIRECTION"),
                    pShape->asDouble("CAPACITY"),
					pShape
					);
			}
			for (iShape=0; iShape<pPoints->Get_Count() && SG_UI_Process_Set_Progress(iShape, pPoints->Get_Count()); iShape++)
			{
				pShape = pPoints->Get_Shape(iShape);
                if ((pNode = GetNode(pShape->asInt("ID"))) != NULL)
				{
					pNode->pShape = pShape;
				}
			}
			return 1;
		}
	}
	return 0;

}
int CNetwork::ValidateVertices(CSG_Table	*pTable)
{
	CSG_String FieldName;
	int iField;
	
	for (iField = 0; iField < pTable->Get_Field_Count(); iField++)
	{
		FieldName = pTable->Get_Field_Name(iField);
		if (!FieldName.Cmp(_TL("ID")))
		{
			return 1;
		}
	}
	return 0;
}

int CNetwork::ValidateTopology(CSG_Table	*pTable)
{
	bool ID,O,D,Impedance,Direction,Capacity;
	CSG_String FieldName;
	int iField;
	
	ID = O = D = Impedance = Direction = Capacity = false;
	for (iField = 0; iField < pTable->Get_Field_Count(); iField++)
	{
		FieldName = pTable->Get_Field_Name(iField);
		if (!FieldName.Cmp(_TL("ID")))
			ID = true;
		else if(!FieldName.Cmp(_TL("O")))
			O = true;
		else if(!FieldName.Cmp(_TL("D")))
			D = true;
		else if(!FieldName.Cmp(_TL("IMPEDANCE")))
			Impedance = true;
		else if(!FieldName.Cmp(_TL("DIRECTION")))
			Direction = true;
		else if(!FieldName.Cmp(_TL("CAPACITY")))
			Capacity = true;
	}
	if (ID && O && D && Impedance && Direction && Capacity)
		return 1;
	else
		return 0;
}

int CNetwork::GetNearNodeID(double x, double y, double radius, int index)
{
	CSG_Shape	*pPoint;
	static double old_radius=0;

	if (pSearch)
	{
		if( radius==0 && (pPoint = pSearch->Get_Point_Nearest(x, y)) != NULL )
		{
			old_radius = 0;
			return pPoint->asInt(_TL("ID"));
		}
		if(radius !=0)
		{
			if (radius==old_radius)
				pSearch->Select_Radius(x,y,radius,true);
			if((pPoint = pSearch->Get_Selected_Point(index)) != NULL)
				return pPoint->asInt(_TL("ID"));
		}
	}
	return 0;
}
double CNetwork::GetPathLength(int StartID, int EndID)
{
	if (StartID>0 && EndID>0)
	{
		if(CreateSTPTreeTo(StartID,EndID))	
			return GetPathLength(EndID);
		else
			SG_UI_Msg_Add_Error(_TL("Can't Create STP Tree"));
	}else{
		SG_UI_Msg_Add_Error(_TL("Invalid ID"));
	}
	return -1;
}
double CNetwork::GetPathLength(double x1, double y1, double x2, double y2)
{
	CSG_Shape	*pPoint1, *pPoint2;
	TSG_Point Point1,Point2;
	double RetVal;
	int NodeIDO,NodeIDD;
	
	Point1.x = x1;
	Point1.y = y1;
	Point2.x = x2;
	Point2.y = y2;
	if (!pSearch)
		return -1;
	if( (pPoint1 = pSearch->Get_Point_Nearest(x1, y1)) != NULL )
	{
		NodeIDO = pPoint1->asInt(_TL("ID"));
		if( (pPoint2 = pSearch->Get_Point_Nearest(x2, y2)) != NULL )
		{
			NodeIDD = pPoint2->asInt(_TL("ID"));
			RetVal = GetPathLength(NodeIDO,NodeIDD);
			RetVal += pPoint1->Get_Distance(Point1)+pPoint2->Get_Distance(Point2);
			return RetVal;
		}else
		{
			SG_UI_Msg_Add_Error(_TL("Can't find nearest end point"));
		}
	}else{
		SG_UI_Msg_Add_Error(_TL("Can't find nearest start point"));
	}	

	return -1;
}

void CNetwork::Insert_A_Link(int lid, int lo, int ld, double ldis, double capa, set<node>& target, CSG_Table_Record *pL)
{
	node tmp_node;
	link *p_tmp_link,tmp_link;
	pair<set<node>::iterator,bool> tmp_pair_node;
	pair<set<link>::iterator,bool> tmp_pair_link;
	
	p_tmp_link=NULL;
	memset(&tmp_node,0,sizeof(node));
	memset(&tmp_link,0,sizeof(link));
	
	tmp_link.ID = lid;
	tmp_link.pRecord = pL;
    //extra johan
    tmp_link.o = lo;
    tmp_link.ld = ld;
    tmp_link.dis = ldis;
    tmp_link.t0 =ldis;
    tmp_link.capa = capa;

	tmp_pair_link=set_Link.insert(set<link>::value_type(tmp_link));

//    tmp_pair_link.first->o=lo;
//    tmp_pair_link.first->ld=ld;
//    tmp_pair_link.first->dis=ldis;
//    tmp_pair_link.first->t0=ldis;
//    tmp_pair_link.first->capa=capa;

	tmp_node.ID = lo;
	tmp_pair_node=target.insert(set<node>::value_type(tmp_node));
	
	tmp_link.next = tmp_pair_node.first->link;
	if (!tmp_pair_link.second)
	{
		for (p_tmp_link = &tmp_link;p_tmp_link->next!=NULL;p_tmp_link=p_tmp_link->next)
		{
			if (p_tmp_link->next->ID==lid)
			{
				p_tmp_link->next = p_tmp_link->next->next;
				break;
			}
		}
	}
	for (p_tmp_link = &tmp_link;p_tmp_link!=NULL;p_tmp_link=p_tmp_link->next)
	{
		if(p_tmp_link->next==NULL || ldis < p_tmp_link->next->dis)
		{
			tmp_pair_link.first->next = p_tmp_link->next;
			p_tmp_link->next = &*(tmp_pair_link.first);
			break;
		}
	}
	tmp_pair_node.first->link = tmp_link.next;
	
	tmp_node.ID = ld;
	tmp_pair_link.first->d = &*(target.insert(set<node>::value_type(tmp_node)).first);
}
int	CNetwork::InsLink(int LinkID,int O,int D,double Impedance,int Direction, double capa, CSG_Table_Record *pL)
{
	
	if(LinkID<1||O<1||D<1||Impedance<0||capa<=0){
//		SG_UI_Dlg_Message(CSG_String::Format("%d,%d,%d,%d,%d",LinkID,O,D,Impedance,Direction),NULL);
		return 0;
	}
	
	if (LinkID>MaxLinkID)
		MaxLinkID = LinkID;
	if (O>MaxNodeID)
		MaxNodeID = O;
	if (D>MaxNodeID)
		MaxNodeID = D;

	switch(Direction) {
	case 0:
		Insert_A_Link(LinkID,O,D,Impedance,capa,set_Node, pL);
		Insert_A_Link(-LinkID,D,O,Impedance,capa,set_Node, pL);
		break;
	case 1:
		Insert_A_Link(LinkID,O,D,Impedance,capa,set_Node, pL);
		break;
	case -1:
		Insert_A_Link(LinkID,D,O,Impedance,capa,set_Node, pL);
		break;
	default:
		return 0;
	}
	return 1;
}
int	CNetwork::CreateSTPTree(int RootID)
{
	register double t_pv;
	register node *d_n,*c_n,*t_n;
	register link *c_l;
// 	set<node> *n = &set_Node;
	set<node>::iterator itr_Node;
	node tmp_node;
	

	tmp_node.ID = RootID;
	itr_Node = set_Node.find(tmp_node);
	if(itr_Node==set_Node.end())
	{
		return 0;
	}
	//SG_UI_Dlg_Message(CSG_String::Format("%d",itr_Node->ID),NULL);
	c_n = &*itr_Node;
	
	
	for(itr_Node=set_Node.begin();itr_Node!=set_Node.end();itr_Node++)
	{
		itr_Node->sign=false;
		itr_Node->pi=NULL;
		itr_Node->pl=NULL;
		itr_Node->pv=INFI;
		itr_Node->ln=&node_Tail;
		itr_Node->rn=&node_Head;
	}
	

	c_n->sign = true; c_n->pi=c_n; c_n->pv = 0;
	c_n->ln=&node_Head;	c_n->rn=&node_Tail;
	node_Head.rn=c_n;	node_Tail.ln=c_n;
	
	for(;c_n->pi!=NULL;c_n=c_n->rn,c_n->sign=true)
	{
		for(c_l=c_n->link;c_l!=NULL;c_l=c_l->next){
			d_n = c_l->d;
			if(!d_n->sign){
				t_pv = c_n->pv + c_l->dis;
				if(t_pv < d_n->pv){
					d_n->pv = t_pv;
					d_n->pi = c_n;
					d_n->pl = c_l;
					for(t_n=d_n->ln;t_pv<t_n->pv;t_n=t_n->ln); 
					if(t_n!=d_n->ln){
						d_n->ln->rn =d_n->rn;
						d_n->rn->ln =d_n->ln;
						d_n->ln =t_n;        d_n->rn =t_n->rn;
						t_n->rn->ln =d_n;    t_n->rn =d_n;
					}
				}
			}
		}
	}
	m_lock=false;
	return 1;
}

int	CNetwork::CreateSTPTree(node *root)
{
	register double t_pv;
	register node *d_n,*c_n,*t_n;
	register link *c_l;
// 	set<node> *n = &set_Node;
	set<node>::iterator itr_Node;
	
	//SG_UI_Dlg_Message(CSG_String::Format("%d",itr_Node->ID),NULL);
	c_n = root;
	
	
	for(itr_Node=set_Node.begin();itr_Node!=set_Node.end();itr_Node++)
	{
		itr_Node->sign=false;
		itr_Node->pi=NULL;
		itr_Node->pl=NULL;
		itr_Node->pv=INFI;
		itr_Node->ln=&node_Tail;
		itr_Node->rn=&node_Head;
	}
	

	c_n->sign = true; c_n->pi=c_n; c_n->pv = 0;
	c_n->ln=&node_Head;	c_n->rn=&node_Tail;
	node_Head.rn=c_n;	node_Tail.ln=c_n;
	
	for(;c_n->pi!=NULL;c_n=c_n->rn,c_n->sign=true)
	{
		for(c_l=c_n->link;c_l!=NULL;c_l=c_l->next){
			d_n = c_l->d;
			if(!d_n->sign){
				t_pv = c_n->pv + c_l->dis;
				if(t_pv < d_n->pv){
					d_n->pv = t_pv;
					d_n->pi = c_n;
					d_n->pl = c_l;
					for(t_n=d_n->ln;t_pv<t_n->pv;t_n=t_n->ln); 
					if(t_n!=d_n->ln){
						d_n->ln->rn =d_n->rn;
						d_n->rn->ln =d_n->ln;
						d_n->ln =t_n;        d_n->rn =t_n->rn;
						t_n->rn->ln =d_n;    t_n->rn =d_n;
					}
				}
			}
		}
	}
	m_lock=false;
	return 1;
}

int CNetwork::CreateSTPTreeTQQ(node *root)
{
	register double t_pv;
	register node *d_n,*c_n,*mid,*tail;
	register link *c_l;
// 	set<node> *n = &set_Node;
	set<node>::iterator itr_Node;
	
	//SG_UI_Dlg_Message(CSG_String::Format("%d",itr_Node->ID),NULL);
	c_n = root;
	
	
	for(itr_Node=set_Node.begin();itr_Node!=set_Node.end();itr_Node++)
	{
		itr_Node->sign=false;
		itr_Node->pi=NULL;
		itr_Node->pl=NULL;
		itr_Node->pv=INFI;
		itr_Node->rn=NULL;
	}
	

	c_n->sign = true; c_n->pi=c_n; c_n->pv = 0;
	mid = c_n; tail = c_n;
	
	for(;c_n; c_n=c_n->rn)
	{
		c_n->sign=true;
		for(c_l=c_n->link;c_l;c_l=c_l->next)
		{
			d_n = c_l->d;
			t_pv = c_n->pv + c_l->dis;
			if(t_pv < d_n->pv)
			{
				if(d_n->pi == NULL)
				{
					tail->rn = d_n;
					tail = d_n;
				}else if(d_n->sign){
					d_n->rn = mid->rn;
					mid->rn = d_n;
					mid = d_n;
					d_n->sign = false;
				}
				d_n->pv = t_pv;
				d_n->pi = c_n;
				d_n->pl = c_l;
			}
		}
		if(mid == c_n)
			mid = c_n->rn;
	}

	m_lock=false;
	return 1;
}

int CNetwork::CreateSTPTreeTQQ(int RootID)
{
	register double t_pv;
	register node *d_n,*c_n,*mid,*tail;
	register link *c_l;
// 	set<node> *n = &set_Node;
	set<node>::iterator itr_Node;
	node tmp_node;
	

	tmp_node.ID = RootID;
	itr_Node = set_Node.find(tmp_node);
	if(itr_Node==set_Node.end())
	{
		return 0;
	}
	//SG_UI_Dlg_Message(CSG_String::Format("%d",itr_Node->ID),NULL);
	c_n = &*itr_Node;	
	
	for(itr_Node=set_Node.begin();itr_Node!=set_Node.end();itr_Node++)
	{
		itr_Node->sign=false;
		itr_Node->pi=NULL;
		itr_Node->pl=NULL;
		itr_Node->pv=INFI;
		itr_Node->rn=NULL;
	}
	

	c_n->sign = true; c_n->pi=c_n; c_n->pv = 0;
	mid = c_n; tail = c_n;
	
	for(;c_n; c_n=c_n->rn)
	{
		c_n->sign=true;
		for(c_l=c_n->link;c_l;c_l=c_l->next)
		{
			d_n = c_l->d;
			t_pv = c_n->pv + c_l->dis;
			if(t_pv < d_n->pv)
			{
				if(d_n->pi == NULL)
				{
					tail->rn = d_n;
					tail = d_n;
				}else if(d_n->sign){
					d_n->rn = mid->rn;
					mid->rn = d_n;
					mid = d_n;
					d_n->sign = false;
				}
				d_n->pv = t_pv;
				d_n->pi = c_n;
				d_n->pl = c_l;
			}
		}
		if(mid == c_n)
			mid = c_n->rn;
	}

	m_lock=false;
	return 1;
}


int CNetwork::CreateSTPTreeTo(int RootID, int StopID)
{
	register double t_pv;
	register node *d_n,*c_n,*t_n;
	register link *c_l;
	set<node>::iterator itr_Node;
	node tmp_node;
	
	tmp_node.ID = RootID;
	itr_Node = set_Node.find(tmp_node);
	if(itr_Node==set_Node.end())
	{
		return 0;
	}
	c_n = &*itr_Node;
	
	
	for(itr_Node=set_Node.begin();itr_Node!=set_Node.end();itr_Node++)
	{
		itr_Node->sign=false;
		itr_Node->pi=NULL;
		itr_Node->pl=NULL;
		itr_Node->pv=INFI;
		itr_Node->ln=&node_Tail;
		itr_Node->rn=&node_Head;
	}
	
	
	c_n->sign = true; c_n->pi=c_n; c_n->pv = 0;
	c_n->ln=&node_Head;	c_n->rn=&node_Tail;
	node_Head.rn=c_n;	node_Tail.ln=c_n;
	
	for(;c_n->pi!=NULL;c_n=c_n->rn,c_n->sign=true)
	{
		if (c_n->ID == StopID)
			break;
		for(c_l=c_n->link;c_l!=NULL;c_l=c_l->next)
		{
			d_n = c_l->d;
			if(!d_n->sign)
			{
				t_pv = c_n->pv + c_l->dis;
				if(t_pv < d_n->pv)
				{
					d_n->pv = t_pv;
					d_n->pi = c_n;
					d_n->pl = c_l;
					for(t_n=d_n->ln;t_pv<t_n->pv;t_n=t_n->ln); 
					if(t_n!=d_n->ln)
					{
						d_n->ln->rn =d_n->rn;
						d_n->rn->ln =d_n->ln;
						d_n->ln =t_n;        d_n->rn =t_n->rn;
						t_n->rn->ln =d_n;    t_n->rn =d_n;
					}
				}
			}
		}
	}
	m_lock=false;
	return 1;
}

int	CNetwork::GetPrecursor(int CurID)
{
	set<node>::iterator itr_Node;
	node tmp_node;
	
	tmp_node.ID=CurID;
	if(CurID>0 && !m_lock)
	{
		itr_Node = set_Node.find(tmp_node);
		if(itr_Node!=set_Node.end())
		{
			return itr_Node->pi!=NULL ? itr_Node->pi->ID : -1;
		}
	}
	return -1;
}
int CNetwork::GetPrelink(int CurID)
{
	set<node>::iterator itr_Node;
	node tmp_node;
	
	tmp_node.ID=CurID;
	if(CurID>0 && !m_lock)
	{
		itr_Node = set_Node.find(tmp_node);
		if(itr_Node!=set_Node.end())
		{
			return itr_Node->pl!=NULL ? itr_Node->pl->ID : -1;
		}
	}
	return -1;
}
double CNetwork::GetPathLength(int CurID)
{
	set<node>::iterator itr_Node;
	node tmp_node;
	
	tmp_node.ID=CurID;
	if(CurID>0 && !m_lock)
	{
		itr_Node = set_Node.find(tmp_node);
		if(itr_Node!=set_Node.end())
		{
			return itr_Node->pv;
		}
	}
	return -1;
}
int	CNetwork::GetLinkID(int O, int D)
{
	set<node>::iterator itr_Node;
	node tmp_node;
	link *p_tmp_link;
	
	tmp_node.ID = O;
	itr_Node = set_Node.find(tmp_node);
	if(itr_Node!=set_Node.end())
		for(p_tmp_link=itr_Node->link;p_tmp_link!=NULL;p_tmp_link=p_tmp_link->next)
			if(p_tmp_link->d->ID==D)
				return abs(p_tmp_link->ID);
	return 0;
}

int	CNetwork::SaveTopoFile(SG_Char FileName[])
{
	set<link>::iterator link_Iter, link_eIter;
	int N,L;
	FILE *fp;
	if((fp=SG_FILE_OPEN(FileName,_TL("wb")))==NULL)
	{
// 		Error_Set(_TL("No such file!"));
		return 0;
	}
	if(set_Link.size() == 0)
	{
// 		Error_Set(_TL("No Links can be saved"));
		return -1;
	}
	N = set_Node.size();
	L = set_Link.size();
	fwrite(&N,4,1,fp);
	fwrite(&L,4,1,fp);
	link_Iter = set_Link.begin();
	link_eIter = set_Link.end();
	while (link_Iter!=link_eIter)
	{
		fwrite(&(link_Iter->ID),4,1,fp);
		fwrite(&(link_Iter->o),4,1,fp);
		fwrite(&(link_Iter->ld),4,1,fp);
		fwrite(&(link_Iter->dis),8,1,fp);
		fwrite(&(link_Iter->capa),8,1,fp);
		link_Iter++;
	}
	fclose(fp);
	return 1;
}

int	CNetwork::LoadTopoFile(SG_Char FileName[])
{
	int i,lid,lo,ld,N,L;
	double ldis, lcapa;
	FILE *fp;
	if((fp=SG_FILE_OPEN(FileName,_TL("rb")))==NULL)
	{
// 		Error_Set(_TL("No such file!"));
		return 0;
	}
	fread(&N,4,1,fp);
	fread(&L,4,1,fp);
	for(i=1;i<=L;i++)
	{
		fread(&lid,4,1,fp);
		fread(&lo,4,1,fp);
		fread(&ld,4,1,fp);
		fread(&ldis,8,1,fp);
		fread(&lcapa,8,1,fp);
		Insert_A_Link(lid,lo,ld,ldis,lcapa,set_Node);
	}
	fclose(fp);
	return 1;
}

int CNetwork::DelLink( int LinkID)
{
	link tmp_link;
	node tmp_node;
	set<link>::iterator itr_Link;
	set<node>::iterator itr_Node;
	link *p_tmp_link;
	int OID, DID;
	m_lock = true;
	if (LinkID > 0)
	{
		memset(&tmp_link,0,sizeof(link));
		memset(&tmp_node,0,sizeof(node));
		tmp_link.ID = LinkID;
		itr_Link = set_Link.find(tmp_link);
		if(itr_Link!=set_Link.end())
		{
			OID = itr_Link->o;
			DID = itr_Link->d->ID;

			tmp_node.ID = OID;
			itr_Node = set_Node.find(tmp_node);
			if(itr_Node!=set_Node.end())
			{
				if (itr_Node->link->d->ID==DID)
				{
					itr_Node->link = itr_Node->link->next;
				}else{
					for(p_tmp_link=itr_Node->link;p_tmp_link!=NULL;p_tmp_link=p_tmp_link->next)
					{
						if (p_tmp_link->next!=NULL && p_tmp_link->next->d->ID==DID)
						{
							p_tmp_link->next = p_tmp_link->next->next;
							break;
						}
					}
				}
			}
			tmp_node.ID = DID;
			itr_Node = set_Node.find(tmp_node);
			if(itr_Node!=set_Node.end())
			{
				if (itr_Node->link->d->ID==OID)
				{
					itr_Node->link = itr_Node->link->next;
				}else{
					for(p_tmp_link=itr_Node->link;p_tmp_link!=NULL;p_tmp_link=p_tmp_link->next)
					{
						if (p_tmp_link->next!=NULL && p_tmp_link->next->d->ID == OID)
						{
							p_tmp_link->next = p_tmp_link->next->next;
							break;
						}
					}
				}
			}
			set_Link.erase(itr_Link);
			tmp_link.ID = -LinkID;
			itr_Link = set_Link.find(tmp_link);
			if (itr_Link!=set_Link.end())
			{
				set_Link.erase(itr_Link);
			}
			return 1;
		}
	}
	return 0;
}

bool CNetwork::Topologize(CSG_Shapes *pInLines, CSG_Shapes *pOutPoints, CSG_Shapes *pOutLines, double tolerance)
{	
	int i=0;
	int iLine, iPart, iPoint;
	CSG_Shape *pInLine, *pOutLine, *pOutPoint;
	CSG_Table *pTable;
	
	Vertex v;
	map<Vertex,int>::iterator itr_v;
	
	m_tolerance = tolerance;
	if (SHAPE_TYPE_Line != pInLines->Get_Type())
	{
		SG_UI_Dlg_Error(_TL("Please open a line layer!"),_TL("Topologize"));
		return false;
	}
	for(iLine=0; iLine<pInLines->Get_Count() && SG_UI_Process_Set_Progress(iLine, pInLines->Get_Count()); iLine++)
	{
		pInLine	= pInLines->Get_Shape(iLine);
		for(iPart=0; iPart<pInLine->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pInLine->Get_Point_Count(iPart); iPoint++)
			{
				v = pInLine->Get_Point(iPoint, iPart);
				if (tolerance!=0)
				{
					v.x = floor(v.x/tolerance+0.5);
					v.y = floor(v.y/tolerance+0.5);
				}
				itr_v = vertices.find(v);
				if(itr_v==vertices.end())
					itr_v=vertices.insert(map<Vertex,int>::value_type(v,0)).first;
				else if(itr_v->second == 0)
					itr_v->second = ++MaxNodeID;
			}
		}
	}
	
	pTable	= pInLines;
	for (i=0; i<pTable->Get_Field_Count(); i++){
		pOutLines->Add_Field(pTable->Get_Field_Name(i), pTable->Get_Field_Type(i));	
	}
//reconstruct table
	bool ID,O,D,Impedance,Direction,Capacity;
	CSG_String FieldName;
	int iField;
	
	ID = O = D = Impedance = Direction = Capacity = false;
	for (iField = 0; iField < pTable->Get_Field_Count(); iField++)
	{
		FieldName = pTable->Get_Field_Name(iField);
		if (!FieldName.Cmp(_TL("ID")))
			ID = true;
		else if(!FieldName.Cmp(_TL("O")))
			O = true;
		else if(!FieldName.Cmp(_TL("D")))
			D = true;
		else if(!FieldName.Cmp(_TL("IMPEDANCE")))
			Impedance = true;
		else if(!FieldName.Cmp(_TL("DIRECTION")))
			Direction = true;
		else if(!FieldName.Cmp(_TL("CAPACITY")))
			Capacity = true;
	}
	if(!ID)
		pOutLines->Add_Field(_TL("ID"),TABLE_FIELDTYPE_Int);
	if(!O)
		pOutLines->Add_Field(_TL("O"),TABLE_FIELDTYPE_Int);
	if(!D)
		pOutLines->Add_Field(_TL("D"),TABLE_FIELDTYPE_Int);
	if(!Impedance)
		pOutLines->Add_Field(_TL("IMPEDANCE"),TABLE_FIELDTYPE_Double);
	if(!Direction)
		pOutLines->Add_Field(_TL("DIRECTION"),TABLE_FIELDTYPE_Int);
	if(!Capacity)
		pOutLines->Add_Field(_TL("CAPACITY"),TABLE_FIELDTYPE_Double);


	if (!ValidateVertices(pOutPoints))
	{
		pOutPoints->Add_Field(_TL("ID"),TABLE_FIELDTYPE_Int);
	}
	
	for(iLine=0; iLine<pInLines->Get_Count() && SG_UI_Process_Set_Progress(iLine, pInLines->Get_Count()); iLine++)
	{
		pInLine	= pInLines->Get_Shape(iLine);
		for(iPart=0; iPart<pInLine->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pInLine->Get_Point_Count(iPart); iPoint++)
			{
				v = pInLine->Get_Point(iPoint, iPart);
				if (tolerance!=0)
				{
					v.x = floor(v.x/tolerance+0.5);
					v.y = floor(v.y/tolerance+0.5);
				}
				itr_v = vertices.find(v);
				if(itr_v!=vertices.end())
				{
					if( 0 == pInLine->Get_Point_Count(iPart)-1)
					{
						break;
					}else if (iPoint==0)
					{
						pOutLine	= pOutLines->Add_Shape(pInLine,SHAPE_COPY_ATTR);
						pOutLine->Add_Point(pInLine->Get_Point(iPoint,iPart));
						
						if (itr_v->second==0)
							itr_v->second = ++MaxNodeID;
	 //	char message[20];
		//sprintf(message,"\t%d,\t%d",/*v.x,v.y,*/MaxNodeID,pInLine->Get_Point_Count(iPart));
		//SG_UI_Msg_Add_Execution(message,true);	

						pOutLine->Set_Value(_TL("O"),itr_v->second);	
					}else if(iPoint!=pInLine->Get_Point_Count(iPart)-1)
					{
						if (itr_v->second==0)
						{
							pOutLine->Add_Point(pInLine->Get_Point(iPoint,iPart));
						}else
						{
							pOutLine->Add_Point(pInLine->Get_Point(iPoint,iPart));
							pOutLine->Set_Value(_TL("D"),itr_v->second);
							
							pOutLine	= pOutLines->Add_Shape(pInLine,SHAPE_COPY_ATTR);
							pOutLine->Add_Point(pInLine->Get_Point(iPoint,iPart));
							pOutLine->Set_Value(_TL("O"),itr_v->second);
						}
					}else if (iPoint==pInLine->Get_Point_Count(iPart)-1)
					{
						pOutLine->Add_Point(pInLine->Get_Point(iPoint,iPart));
						if (itr_v->second==0)
							itr_v->second = ++MaxNodeID;
						pOutLine->Set_Value(_TL("D"),itr_v->second);	
					}
				}
			}
		}
	}
	
	for(iLine=0; iLine<pOutLines->Get_Count() && SG_UI_Process_Set_Progress(iLine, pOutLines->Get_Count()); iLine++)
	{
		pOutLine	= pOutLines->Get_Shape(iLine);
		pOutLine->Set_Value(_TL("ID"), iLine+1);
		if (IsSpherical)
		{
			int iPart,iPoint;
			double x1,y1,x2,y2,dLat,dLon,a,c,Length;
			double EarthR = 6371;
			
			Length = 0;
			for (iPart=0; iPart<pOutLine->Get_Part_Count(); iPart++)
			{
				x1 = pOutLine->Get_Point(0,iPart).x;
				y1 = pOutLine->Get_Point(0,iPart).y;									
				for(iPoint=1; iPoint<pOutLine->Get_Point_Count(iPart); iPoint++)
				{
					x2 = pOutLine->Get_Point(iPoint,iPart).x;
					y2 = pOutLine->Get_Point(iPoint,iPart).y;
					dLat = M_PI*(y2-y1)/180;
					dLon = M_PI*(x2-x1)/180;
					a = sin(dLat/2)*sin(dLat/2)+cos(M_PI*y1/180)*cos(M_PI*y2/180)*sin(dLon/2)*sin(dLon/2);
					c = 2 * atan2(sqrt(a),sqrt(1-a));
					Length	+= EarthR * c;
					x1 = x2;
					y1 = y2;
				}
			}
			pOutLine->Set_Value(_TL("IMPEDANCE"),Length);
		}else{
			pOutLine->Set_Value(_TL("IMPEDANCE"),((CSG_Shape_Line *)pOutLine)->Get_Length());
		}
		pOutLine->Set_Value(_TL("CAPACITY"),INFI);
	}
	
	for (itr_v=vertices.begin();itr_v!=vertices.end();itr_v++)
	{
		if (itr_v->second!=0)
		{
			pOutPoint = pOutPoints->Add_Shape();
			if (tolerance!=0)
				pOutPoint->Add_Point(itr_v->first.x*tolerance,itr_v->first.y*tolerance);
			else
				pOutPoint->Add_Point(itr_v->first);
			pOutPoint->Set_Value(_TL("ID"),itr_v->second);
		}
	}
	return true;
}

int CNetwork::AllocNodeID(double x, double y)
{
	Vertex v;
	map<Vertex,int>::iterator itr_v;
	if (m_tolerance!=0)
	{
		v.x = floor(x/m_tolerance+0.5);
		v.y = floor(y/m_tolerance+0.5);
	}
	itr_v = vertices.find(v);
	if(itr_v==vertices.end())
		itr_v=vertices.insert(map<Vertex,int>::value_type(v,0)).first;
	else if(itr_v->second == 0)
		itr_v->second = ++MaxNodeID;

	return itr_v->second;
}

node * CNetwork::GetNode( int NodeID )
{
	set<node>::iterator itr_Node;
	node tmp_node;
	
	tmp_node.ID=NodeID;
	if(NodeID>0)
	{
		itr_Node = set_Node.find(tmp_node);
		if(itr_Node!=set_Node.end())
		{
			return &*itr_Node;
		}
	}
	return NULL;

}
link * CNetwork::GetLink(int LinkID)
{
	set<link>::iterator itr_Link;
	link tmp_link;

	tmp_link.ID = LinkID;
	if (LinkID>0)
	{
		itr_Link = set_Link.find(tmp_link);
		if(itr_Link!=set_Link.end())
		{
			return &*itr_Link;
		}
	}
	return NULL;
}

int CNetwork::SetTrip(int O, int D, double amount)
{
	otrip tmp_otrip;
	dtrip tmp_dtrip;
	set<node>::iterator itr_Node;
	pair<set<otrip>::iterator,bool> tmp_pair_otrip;
	pair<set<dtrip>::iterator,bool> tmp_pair_dtrip;
	node tmp_node;
	
	if (O<=0 || D<=0 || amount<=0)
		return 0;

	tmp_node.ID=O;
	itr_Node = set_Node.find(tmp_node);
	if(itr_Node!=set_Node.end())
	{
		tmp_otrip.o = &*itr_Node;
	}else{
		return -1;
	}

	tmp_node.ID=D;
	itr_Node = set_Node.find(tmp_node);
	if(itr_Node!=set_Node.end())
	{
		tmp_dtrip.d = &*itr_Node;
	}else{
		return -2;
	}

	tmp_pair_otrip = trips.insert(set<otrip>::value_type(tmp_otrip));
	tmp_pair_dtrip = tmp_pair_otrip.first->d.insert(set<dtrip>::value_type(tmp_dtrip));
	tmp_pair_dtrip.first->amount = amount;
	return 1;
}

int CNetwork::AssignTraffic(double tol_root, double tol_final, int max_iter)
{
	set<link>::iterator itr_link;
	set<otrip>::iterator itr_otrip;
	set<dtrip>::iterator itr_dtrip;
	node *pnode;

	//eps_tolerance<double> termc(10);
	//uintmax_t miter;
	//pair<double,double> pair_lambda;
	double _lambda;
	double total_cost = 0;
	double crit_numerator, crit_denominator;
	bool solved = false;
	_lambda = 1.0f;
	for(itr_link = set_Link.begin();itr_link!=set_Link.end();itr_link++)
	{
		itr_link->flow = 0;
		itr_link->dflow = 0;
	}
	for(int i = 0; i<max_iter; i++)
	{
		for(itr_otrip = trips.begin(); itr_otrip != trips.end(); itr_otrip++)
		{
			CreateSTPTreeTQQ(itr_otrip->o);
			for(itr_dtrip = itr_otrip->d.begin(); itr_dtrip != itr_otrip->d.end(); itr_dtrip++)
			{
				pnode = itr_dtrip->d;
				if(pnode->pl != NULL)
				{
					for(; pnode->pl != NULL; pnode=pnode->pi)
					{
						pnode->pl->dflow += itr_dtrip->amount;
					}
				}
			}
		}
		if(i==0)
		{
			for(itr_link = set_Link.begin();itr_link!=set_Link.end();itr_link++)
			{
				itr_link->flow = itr_link->dflow;
				itr_link->dflow = 0;
				itr_link->dis = itr_link->t0*(1 + 0.15f*pow(itr_link->flow/itr_link->capa,4));
			}
		}else{
			//miter = 1000;
			//function<double (double)>  myfunc = bind(&CNetwork::DZ, *this, _1);
			//pair_lambda = toms748_solve<CNetwork,double,eps_tolerance<double>>(*this,0.0f,1.0f,termc,miter);
			//_lambda = (pair_lambda.first + pair_lambda.second)*0.5f;
			_lambda = toms748_solve(0.0f,1.0f,tol_root,10000);
			crit_numerator = crit_denominator = 0;
			//SG_UI_Msg_Add_Execution(CSG_String::Format("%d",miter),true);

			for(itr_link = set_Link.begin();itr_link!=set_Link.end();itr_link++)
			{
				crit_numerator += _lambda*_lambda*(itr_link->dflow - itr_link->flow)*(itr_link->dflow - itr_link->flow);
				crit_denominator += itr_link->flow;
				itr_link->flow += _lambda*(itr_link->dflow - itr_link->flow);
				itr_link->dflow = 0;
				itr_link->dis = itr_link->t0*(1 + 0.15f*pow(itr_link->flow/itr_link->capa,4));
			}
			if( sqrt(crit_numerator)/crit_denominator < tol_final)
				return 1;
			if(i%100==0)
			{
				if(SG_UI_Process_Get_Okay())
				{
					SG_UI_Process_Set_Text(CSG_String::Format("%d",i));
					SG_UI_Process_Set_Progress(crit_denominator /sqrt(crit_numerator),1/tol_final);
				}else{
					return 0;
				}
			}
		}
	}
	return 0;
}

double CNetwork::Sigma(double _lambda)
{
#define alpha 0.15f
#define beta  4
	double result = 0;

	set<link>::iterator itr_link;
	for(itr_link = set_Link.begin();itr_link!=set_Link.end();itr_link++)
	{
		//if(itr_link->dflow || itr_link->flow)
			result +=(itr_link->dflow - itr_link->flow)*itr_link->t0*(1+alpha*pow(((itr_link->flow + _lambda*(itr_link->dflow - itr_link->flow))/itr_link->capa),beta));
		//static int i=0;
		//if(i<100)
		//{
		//	SG_UI_Msg_Add_Error(CSG_String::Format("%d,%f,%f,%f,%f,%f",itr_link->ID,itr_link->dflow,itr_link->flow,itr_link->t0,itr_link->capa,result));
		//	i++;
		//}
	}
	return result;
}

double CNetwork::GetTraffic(int linkID)
{
	set<link>::iterator itr_Link;
	link tmp_link;

	tmp_link.ID = linkID;
	itr_Link = set_Link.find(tmp_link);
	if(itr_Link!=set_Link.end())
	{
		return itr_Link->flow;
	}else{
		return -1;
	}
}
