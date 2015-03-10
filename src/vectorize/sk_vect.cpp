/*Header-MicMac-eLiSe-25/06/2007

    MicMac : Multi Image Correspondances par Methodes Automatiques de Correlation
    eLiSe  : ELements of an Image Software Environnement

    www.micmac.ign.fr

   
    Copyright : Institut Geographique National
    Author : Marc Pierrot Deseilligny
    Contributors : Gregoire Maillet, Didier Boldo.

[1] M. Pierrot-Deseilligny, N. Paparoditis.
    "A multiresolution and optimization-based image matching approach:
    An application to surface reconstruction from SPOT5-HRS stereo imagery."
    In IAPRS vol XXXVI-1/W41 in ISPRS Workshop On Topographic Mapping From Space
    (With Special Emphasis on Small Satellites), Ankara, Turquie, 02-2006.

[2] M. Pierrot-Deseilligny, "MicMac, un lociel de mise en correspondance
    d'images, adapte au contexte geograhique" to appears in 
    Bulletin d'information de l'Institut Geographique National, 2007.

Francais :

   MicMac est un logiciel de mise en correspondance d'image adapte 
   au contexte de recherche en information geographique. Il s'appuie sur
   la bibliotheque de manipulation d'image eLiSe. Il est distibue sous la
   licences Cecill-B.  Voir en bas de fichier et  http://www.cecill.info.


English :

    MicMac is an open source software specialized in image matching
    for research in geographic information. MicMac is built on the
    eLiSe image library. MicMac is governed by the  "Cecill-B licence".
    See below and http://www.cecill.info.

Header-MicMac-eLiSe-25/06/2007*/

#include "StdAfx.h"
#include "ext_stl/pack_list.h"

#define NoOperatorVirgule



/*
     Bench :
        

      * a faire sur skelette (cas "realiste") et sur graphe aleatoire
      (mais symetrique) car a priri algo fonctionne sur tous graphe, symetrique,
      sous-graphe du 8-Voisinage.


     Bench :

       *  verfifier pour chaque brin que :

          [o] tous les  cple de sommets consec sont des arcs
          [o] les extres sont des neud de degres != 2
          [o] les non extres sont des noeuds de degres 2
          [o] syntetiser le graphe
*/

#define NB_PT   6
#define NB_ATTR 6



typedef ElPackList<Pt2di,NB_PT>    SkVectLPts;
typedef ElPackList<INT,NB_ATTR>    SkVectLAttr;


class Br_Skel_Vect : public Mcheck
{
      friend class Vector_Skel;

      public :

          Br_Skel_Vect(SkVectLPts *,SkVectLAttr *,INT nb_attr);
          bool extre_is_first(Pt2di p);
          virtual ~Br_Skel_Vect();

      private :

          SkVectLPts                 _pts;
          SkVectLAttr                _attr;
          INT                         _nb_extre_free;
          INT                         _nb_attr;
};



bool Br_Skel_Vect::extre_is_first(Pt2di p)
{
     if (_pts.first()== p)
        return true;

     ELISE_ASSERT 
     (
        _pts.last() == p,
        "Incoherence , Br_Skel_Vect::pt_is_first"
     );
     return false;
}


Br_Skel_Vect::Br_Skel_Vect
(
     SkVectLPts *  ResPts,
     SkVectLAttr * ResAttr,
     INT nb_attr
) :
    _pts           (ResPts),
    _attr          (ResAttr),
    _nb_extre_free (2),
    _nb_attr       (nb_attr)
{
}



Br_Skel_Vect::~Br_Skel_Vect()
{
    // A Optimiser + tard avec un vidage de liste direct
    while (! (_pts.empty()))
    {
        _pts.pop_front();
        for (INT k=0; k< _nb_attr ; k++)
           _attr.pop_front();
    }
}




class Vector_Skel : public Simple_OPBuf1<INT,INT>
{
     public :

		

       virtual ~Vector_Skel();
       Simple_OPBuf1<INT,INT> * dup_comp();

       Vector_Skel(Br_Vect_Action *);

     private :


       bool is_vertex(Pt2di p)
       {
           return SkVein::NbBitFlag[_flag[p.y-ycur()][p.x]] != 2;
       }

       bool is_vertex_gen(Pt2di p)
       {
           return (p.y < ycur()) || (is_vertex(p));
       }
      

      Fonc_Num adapt_box(Fonc_Num f,Box2di b)
      {
          return nflag_open_sym_id(clip_def(Iconv(f),0,b._p0,b._p1));
      }


      virtual void  calc_buf(INT ** output,INT *** input);


       void   pushlast_br_pt(Br_Skel_Vect * br,Pt2di pt)
       {
               br->_pts.push_back(pt); 
               for (INT d=1;d<dim_in();d++)
                   br->_attr. push_back(_in[d][pt.y-ycur()][pt.x]);
       }

       void   pushfirst_br_pt(Br_Skel_Vect * br,Pt2di pt)
       {
               br->_pts.push_front(pt); 
               for (INT d=dim_in()-1;d>=1;d--)
                   br->_attr.push_front(_in[d][pt.y-ycur()][pt.x]);
       }



       Br_Skel_Vect * get_br(Pt2di p)
       {
             return _brs[p.y-ycur()][p.x];
       }

       void index_br(Br_Skel_Vect * br,Pt2di p);
       void unidx(Pt2di p);
       void  recycler(Br_Skel_Vect * br);

       Br_Skel_Vect * create_new_br(Pt2di p);
       void add_arc(INT x,INT k);
       void action_br(Br_Skel_Vect *,bool cycle);

       void  add_arc(Br_Skel_Vect *,Pt2di p1,Pt2di p2);
       void  merge(Br_Skel_Vect *br1,Pt2di p1,Br_Skel_Vect * br2,Pt2di p2);


      Br_Skel_Vect ***       _brs;
      Pt2di               _p0_brs;
      Pt2di               _p1_brs;
      INT **                _flag;
      Br_Vect_Action *       _act;
      PRC0               _prc_act;

      INT    *_attrs;

      ElFifo<Pt2di>              _Fpts;
      ElFifo<INT> *              _Fattr;

      SkVectLPts                 _BufPts;
      SkVectLAttr                _BufAttr;
      ElFilo<Br_Skel_Vect *>     _BufBsv;
};



Simple_OPBuf1<INT,INT> * Vector_Skel::dup_comp() 
{


   Vector_Skel * res = new Vector_Skel(_act);

   res->_p0_brs = Pt2di(x0()-1,0);
   res->_p1_brs = Pt2di(x1()+1,2);
   res->_brs = NEW_MATRICE(res->_p0_brs,res->_p1_brs,Br_Skel_Vect *);
   for (INT x = res->_p0_brs.x; x<res->_p1_brs.x ; x++)
       for (INT y = res->_p0_brs.y; y<res->_p1_brs.y ; y++)
           res->_brs[y][x] = 0;

   res->_attrs  = NEW_VECTEUR(0,dim_in(),INT);

   res->_Fattr = NEW_TAB (dim_in()-1,ElFifo<INT>);

   return res;
}

Vector_Skel::~Vector_Skel() 
{
    if (_brs)
    {
        for (INT x = _p0_brs.x; x<_p1_brs.x ; x++)
            for (INT y = _p0_brs.y; y<_p1_brs.y ; y++)
                if (_brs[y][x])
                   DELETE_ONE (_brs[y][x]);
        DELETE_MATRICE(_brs,_p0_brs,_p1_brs);
        DELETE_VECTOR(_attrs,0);
        for (INT  k =0; k<_BufBsv.nb() ; k++)
            DELETE_ONE(_BufBsv[k]);
        DELETE_TAB(_Fattr);
    }
}


Vector_Skel::Vector_Skel(Br_Vect_Action * act) :
    _brs     (0),
    _flag    (0),
    _act     (act),
    _prc_act (act)
{
}


void Vector_Skel::index_br(Br_Skel_Vect * br,Pt2di p)
{

     if (is_vertex_gen (p))
     {
        br->_nb_extre_free--;
        if (br->_nb_extre_free == 0)
           action_br(br,false);
     }
     else
     {
         ELISE_ASSERT
         (
             _brs[p.y-ycur()][p.x] == 0,
             "Incoherence in Vector_Skel::index_br"
         );
        _brs[p.y-ycur()][p.x] = br;
     }
}

void Vector_Skel::unidx(Pt2di p)
{
     ELISE_ASSERT
     (
         _brs[p.y-ycur()][p.x] != 0,
         "Incoherence in Vector_Skel::unidx"
     );
     _brs[p.y-ycur()][p.x] = 0;
}


void Vector_Skel::action_br(Br_Skel_Vect * br,bool cycle)
{
      _Fpts.clear();
      _Fpts.set_circ(cycle);
      for (INT d =0; d<dim_in()-1 ; d++)
      {
          _Fattr[d].clear();
          _Fattr[d].set_circ(cycle);
          
      }

      while(! br->_pts.empty())
      {
           _Fpts.pushlast(br->_pts.pop_front());
           for (INT d =0; d<dim_in()-1 ; d++)
              _Fattr[d].pushlast(br->_attr.pop_front());
      }
      _act->action(_Fpts,_Fattr,dim_in()-1);
     recycler(br);
}


void Vector_Skel::recycler(Br_Skel_Vect * br)
{
   ELISE_ASSERT
   ( 
        br->_pts.empty() && br->_attr.empty(),
        "Vector_Skel::recycler"
   );
   _BufBsv.pushlast(br);
}

Br_Skel_Vect * Vector_Skel::create_new_br(Pt2di p)
{
    Br_Skel_Vect * res = 0;

    if (_BufBsv.empty())
       res = CLASS_NEW_ONE(Br_Skel_Vect,(&_BufPts,&_BufAttr,dim_in()-1));
    else
       res = _BufBsv.poplast();

    pushlast_br_pt(res,p); 
    res->_nb_extre_free = 2;
    index_br(res,p);

    return res;
}

void Vector_Skel::add_arc(Br_Skel_Vect * br,Pt2di p1,Pt2di p2)
{
     if (br->extre_is_first(p1))
        pushfirst_br_pt(br,p2);  
     else
        pushlast_br_pt(br,p2); 

     if (br->_pts.size()> 2)
         unidx(p1);

     index_br(br,p2);
}

void  Vector_Skel::merge
      (
         Br_Skel_Vect *   br1,
         Pt2di            p1,
         Br_Skel_Vect *   br2,
         Pt2di            p2
      )
{
     bool  fp1 = br1->extre_is_first(p1);
     bool  fp2 = br2->extre_is_first(p2);

     Pt2di p ;
     while (! br2->_pts.empty())
     {
          if (fp2)
          {
              p =  br2->_pts.pop_front() ; 
              for (INT d = 0; d<dim_in() -1; d++)
                   _attrs[d]  = br2->_attr.pop_front() ;
          }
          else
          {
              p = br2->_pts.pop_back();
              for (INT d = dim_in()-2 ; d >=0 ; d--)
                   _attrs[d]  = br2->_attr.pop_back() ;
          }


          if (fp1)
          {
              br1->_pts.push_front(p);
              for (INT d = dim_in()-2 ; d >=0 ; d--)
                  br1->_attr.push_front(_attrs[d]);
          }
          else
          {
              br1->_pts.push_back(p);
              for (INT d = 0; d<dim_in() -1; d++)
                   br1->_attr.push_back(_attrs[d]);
          }
     }
     if (! is_vertex_gen(p))
         unidx(p);
     recycler(br2);

     // nb_extre de br1 OK; p1 est comptee, p2 va l'etre si necessaire
     index_br(br1,p);
}

void  Vector_Skel::add_arc(INT x1,INT k)
{
      Pt2di p1(x1,ycur());
      Pt2di p2 = p1 + TAB_8_NEIGH[k];


      Br_Skel_Vect * br1 = get_br(p1);
      Br_Skel_Vect * br2 = get_br(p2);

     
      if ((! br1) && (! br2))
         br1 = create_new_br(p1);

      if (br1 && br2)
      {
           unidx(p1);
           unidx(p2);
           if (br1 == br2)     // treat cycle
           {
                action_br(br1,true);
           }
           else
           {
               if (br1->_pts.size()>br2->_pts.size())
                  merge(br1,p1,br2,p2);
               else
                  merge(br2,p2,br1,p1);
           }
      }
      else
      {
          if (br1)
             add_arc(br1,p1,p2);
          else
             add_arc(br2,p2,p1);
      }
}


void  Vector_Skel::calc_buf (INT ** out, INT *** input)
{

     // Useless, but to make it a real function (for now).
     {
        MEM_RAZ(out[0]+x0(),x1()-x0());
     }


     //
     {
        _flag = input[0];
        INT * f = _flag[0];
        INT X0 = x0();
        INT X1 = x1();
        INT x;
        INT vf;

        for (x =X0; x<X1; x++)
        {
            if (f[x])
            {
               vf = f[x];
               for (INT k = 0; k < 4; k++)
                   if(vf & (1<<k))
                      add_arc(x,k);
            }
       }
     }

      Br_Skel_Vect ** _tmp_br = _brs[0];
      _brs[0]                 = _brs[1];
      _brs[1]                 = _tmp_br;
}


Fonc_Num  sk_vect
          (
               Fonc_Num f,
               Br_Vect_Action * act
          )
{
     return create_op_buf_simple_tpl
            (
                new Vector_Skel (act),
                0,
                f,
                1,
                Box2di(1)
            );
}


/*Footer-MicMac-eLiSe-25/06/2007

Ce logiciel est un programme informatique servant à la mise en
correspondances d'images pour la reconstruction du relief.

Ce logiciel est régi par la licence CeCILL-B soumise au droit français et
respectant les principes de diffusion des logiciels libres. Vous pouvez
utiliser, modifier et/ou redistribuer ce programme sous les conditions
de la licence CeCILL-B telle que diffusée par le CEA, le CNRS et l'INRIA 
sur le site "http://www.cecill.info".

En contrepartie de l'accessibilité au code source et des droits de copie,
de modification et de redistribution accordés par cette licence, il n'est
offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,
seule une responsabilité restreinte pèse sur l'auteur du programme,  le
titulaire des droits patrimoniaux et les concédants successifs.

A cet égard  l'attention de l'utilisateur est attirée sur les risques
associés au chargement,  à l'utilisation,  à la modification et/ou au
développement et à la reproduction du logiciel par l'utilisateur étant 
donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
manipuler et qui le réserve donc à des développeurs et des professionnels
avertis possédant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invités à charger  et  tester  l'adéquation  du
logiciel à leurs besoins dans des conditions permettant d'assurer la
sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 

Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
pris connaissance de la licence CeCILL-B, et que vous en avez accepté les
termes.
Footer-MicMac-eLiSe-25/06/2007*/
