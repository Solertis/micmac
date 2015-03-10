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
// File Automatically generated by eLiSe

#include "general/all.h"
#include "private/all.h"
#include "cEqAppuiXDHom.h"


cEqAppuiXDHom::cEqAppuiXDHom():
    cElCompiledFonc(1)
{
   AddIntRef (cIncIntervale("Intr",0,8));
   AddIntRef (cIncIntervale("Orient",8,14));
   Close(false);
}



void cEqAppuiXDHom::ComputeVal()
{
   double tmp0_ = mCompCoord[8];
   double tmp1_ = mCompCoord[9];
   double tmp2_ = cos(tmp1_);
   double tmp3_ = sin(tmp0_);
   double tmp4_ = cos(tmp0_);
   double tmp5_ = sin(tmp1_);
   double tmp6_ = mCompCoord[10];
   double tmp7_ = mCompCoord[11];
   double tmp8_ = mLocXTer-tmp7_;
   double tmp9_ = sin(tmp6_);
   double tmp10_ = -(tmp9_);
   double tmp11_ = -(tmp5_);
   double tmp12_ = cos(tmp6_);
   double tmp13_ = mCompCoord[12];
   double tmp14_ = mLocYTer-tmp13_;
   double tmp15_ = mCompCoord[13];
   double tmp16_ = mLocZTer-tmp15_;
   double tmp17_ = -(tmp3_);
   double tmp18_ = tmp4_*tmp11_;
   double tmp19_ = tmp3_*tmp11_;
   double tmp20_ = tmp17_*tmp10_;
   double tmp21_ = tmp18_*tmp12_;
   double tmp22_ = tmp20_+tmp21_;
   double tmp23_ = (tmp22_)*(tmp8_);
   double tmp24_ = tmp4_*tmp10_;
   double tmp25_ = tmp19_*tmp12_;
   double tmp26_ = tmp24_+tmp25_;
   double tmp27_ = (tmp26_)*(tmp14_);
   double tmp28_ = tmp23_+tmp27_;
   double tmp29_ = tmp2_*tmp12_;
   double tmp30_ = tmp29_*(tmp16_);
   double tmp31_ = tmp28_+tmp30_;
   double tmp32_ = tmp4_*tmp2_;
   double tmp33_ = tmp32_*(tmp8_);
   double tmp34_ = tmp3_*tmp2_;
   double tmp35_ = tmp34_*(tmp14_);
   double tmp36_ = tmp33_+tmp35_;
   double tmp37_ = tmp5_*(tmp16_);
   double tmp38_ = tmp36_+tmp37_;
   double tmp39_ = (tmp38_)/(tmp31_);
   double tmp40_ = tmp17_*tmp12_;
   double tmp41_ = tmp18_*tmp9_;
   double tmp42_ = tmp40_+tmp41_;
   double tmp43_ = (tmp42_)*(tmp8_);
   double tmp44_ = tmp4_*tmp12_;
   double tmp45_ = tmp19_*tmp9_;
   double tmp46_ = tmp44_+tmp45_;
   double tmp47_ = (tmp46_)*(tmp14_);
   double tmp48_ = tmp43_+tmp47_;
   double tmp49_ = tmp2_*tmp9_;
   double tmp50_ = tmp49_*(tmp16_);
   double tmp51_ = tmp48_+tmp50_;
   double tmp52_ = (tmp51_)/(tmp31_);

  mVal[0] = (mCompCoord[0]*(tmp39_)+mCompCoord[1]*(tmp52_)+mCompCoord[2])/(mCompCoord[6]*(tmp39_)+mCompCoord[7]*(tmp52_)+1)-mLocXIm;

}


void cEqAppuiXDHom::ComputeValDeriv()
{
   double tmp0_ = mCompCoord[8];
   double tmp1_ = mCompCoord[9];
   double tmp2_ = cos(tmp1_);
   double tmp3_ = sin(tmp0_);
   double tmp4_ = cos(tmp0_);
   double tmp5_ = sin(tmp1_);
   double tmp6_ = mCompCoord[10];
   double tmp7_ = mCompCoord[11];
   double tmp8_ = mLocXTer-tmp7_;
   double tmp9_ = sin(tmp6_);
   double tmp10_ = -(tmp9_);
   double tmp11_ = -(tmp5_);
   double tmp12_ = cos(tmp6_);
   double tmp13_ = mCompCoord[12];
   double tmp14_ = mLocYTer-tmp13_;
   double tmp15_ = mCompCoord[13];
   double tmp16_ = mLocZTer-tmp15_;
   double tmp17_ = -(tmp3_);
   double tmp18_ = tmp4_*tmp11_;
   double tmp19_ = tmp3_*tmp11_;
   double tmp20_ = tmp17_*tmp10_;
   double tmp21_ = tmp18_*tmp12_;
   double tmp22_ = tmp20_+tmp21_;
   double tmp23_ = (tmp22_)*(tmp8_);
   double tmp24_ = tmp4_*tmp10_;
   double tmp25_ = tmp19_*tmp12_;
   double tmp26_ = tmp24_+tmp25_;
   double tmp27_ = (tmp26_)*(tmp14_);
   double tmp28_ = tmp23_+tmp27_;
   double tmp29_ = tmp2_*tmp12_;
   double tmp30_ = tmp29_*(tmp16_);
   double tmp31_ = tmp28_+tmp30_;
   double tmp32_ = tmp4_*tmp2_;
   double tmp33_ = tmp32_*(tmp8_);
   double tmp34_ = tmp3_*tmp2_;
   double tmp35_ = tmp34_*(tmp14_);
   double tmp36_ = tmp33_+tmp35_;
   double tmp37_ = tmp5_*(tmp16_);
   double tmp38_ = tmp36_+tmp37_;
   double tmp39_ = (tmp38_)/(tmp31_);
   double tmp40_ = tmp17_*tmp12_;
   double tmp41_ = tmp18_*tmp9_;
   double tmp42_ = tmp40_+tmp41_;
   double tmp43_ = (tmp42_)*(tmp8_);
   double tmp44_ = tmp4_*tmp12_;
   double tmp45_ = tmp19_*tmp9_;
   double tmp46_ = tmp44_+tmp45_;
   double tmp47_ = (tmp46_)*(tmp14_);
   double tmp48_ = tmp43_+tmp47_;
   double tmp49_ = tmp2_*tmp9_;
   double tmp50_ = tmp49_*(tmp16_);
   double tmp51_ = tmp48_+tmp50_;
   double tmp52_ = (tmp51_)/(tmp31_);
   double tmp53_ = mCompCoord[6];
   double tmp54_ = tmp53_*(tmp39_);
   double tmp55_ = mCompCoord[7];
   double tmp56_ = tmp55_*(tmp52_);
   double tmp57_ = tmp54_+tmp56_;
   double tmp58_ = tmp57_+1;
   double tmp59_ = ElSquare(tmp58_);
   double tmp60_ = mCompCoord[0];
   double tmp61_ = tmp60_*(tmp39_);
   double tmp62_ = mCompCoord[1];
   double tmp63_ = tmp62_*(tmp52_);
   double tmp64_ = tmp61_+tmp63_;
   double tmp65_ = mCompCoord[2];
   double tmp66_ = tmp64_+tmp65_;
   double tmp67_ = -(1);
   double tmp68_ = tmp67_*tmp3_;
   double tmp69_ = -(tmp4_);
   double tmp70_ = tmp68_*tmp11_;
   double tmp71_ = tmp69_*tmp10_;
   double tmp72_ = tmp70_*tmp12_;
   double tmp73_ = tmp71_+tmp72_;
   double tmp74_ = (tmp73_)*(tmp8_);
   double tmp75_ = tmp68_*tmp10_;
   double tmp76_ = tmp75_+tmp21_;
   double tmp77_ = (tmp76_)*(tmp14_);
   double tmp78_ = tmp74_+tmp77_;
   double tmp79_ = ElSquare(tmp31_);
   double tmp80_ = tmp68_*tmp2_;
   double tmp81_ = tmp80_*(tmp8_);
   double tmp82_ = tmp32_*(tmp14_);
   double tmp83_ = tmp81_+tmp82_;
   double tmp84_ = (tmp83_)*(tmp31_);
   double tmp85_ = (tmp38_)*(tmp78_);
   double tmp86_ = tmp84_-tmp85_;
   double tmp87_ = (tmp86_)/tmp79_;
   double tmp88_ = tmp69_*tmp12_;
   double tmp89_ = tmp70_*tmp9_;
   double tmp90_ = tmp88_+tmp89_;
   double tmp91_ = (tmp90_)*(tmp8_);
   double tmp92_ = tmp68_*tmp12_;
   double tmp93_ = tmp92_+tmp41_;
   double tmp94_ = (tmp93_)*(tmp14_);
   double tmp95_ = tmp91_+tmp94_;
   double tmp96_ = (tmp95_)*(tmp31_);
   double tmp97_ = (tmp51_)*(tmp78_);
   double tmp98_ = tmp96_-tmp97_;
   double tmp99_ = (tmp98_)/tmp79_;
   double tmp100_ = tmp67_*tmp5_;
   double tmp101_ = -(tmp2_);
   double tmp102_ = tmp101_*tmp4_;
   double tmp103_ = tmp101_*tmp3_;
   double tmp104_ = tmp102_*tmp12_;
   double tmp105_ = tmp104_*(tmp8_);
   double tmp106_ = tmp103_*tmp12_;
   double tmp107_ = tmp106_*(tmp14_);
   double tmp108_ = tmp105_+tmp107_;
   double tmp109_ = tmp100_*tmp12_;
   double tmp110_ = tmp109_*(tmp16_);
   double tmp111_ = tmp108_+tmp110_;
   double tmp112_ = tmp100_*tmp4_;
   double tmp113_ = tmp112_*(tmp8_);
   double tmp114_ = tmp100_*tmp3_;
   double tmp115_ = tmp114_*(tmp14_);
   double tmp116_ = tmp113_+tmp115_;
   double tmp117_ = tmp2_*(tmp16_);
   double tmp118_ = tmp116_+tmp117_;
   double tmp119_ = (tmp118_)*(tmp31_);
   double tmp120_ = (tmp38_)*(tmp111_);
   double tmp121_ = tmp119_-tmp120_;
   double tmp122_ = (tmp121_)/tmp79_;
   double tmp123_ = tmp102_*tmp9_;
   double tmp124_ = tmp123_*(tmp8_);
   double tmp125_ = tmp103_*tmp9_;
   double tmp126_ = tmp125_*(tmp14_);
   double tmp127_ = tmp124_+tmp126_;
   double tmp128_ = tmp100_*tmp9_;
   double tmp129_ = tmp128_*(tmp16_);
   double tmp130_ = tmp127_+tmp129_;
   double tmp131_ = (tmp130_)*(tmp31_);
   double tmp132_ = (tmp51_)*(tmp111_);
   double tmp133_ = tmp131_-tmp132_;
   double tmp134_ = (tmp133_)/tmp79_;
   double tmp135_ = -(tmp12_);
   double tmp136_ = tmp67_*tmp9_;
   double tmp137_ = tmp135_*tmp17_;
   double tmp138_ = tmp136_*tmp18_;
   double tmp139_ = tmp137_+tmp138_;
   double tmp140_ = (tmp139_)*(tmp8_);
   double tmp141_ = tmp135_*tmp4_;
   double tmp142_ = tmp136_*tmp19_;
   double tmp143_ = tmp141_+tmp142_;
   double tmp144_ = (tmp143_)*(tmp14_);
   double tmp145_ = tmp140_+tmp144_;
   double tmp146_ = tmp136_*tmp2_;
   double tmp147_ = tmp146_*(tmp16_);
   double tmp148_ = tmp145_+tmp147_;
   double tmp149_ = (tmp38_)*(tmp148_);
   double tmp150_ = -(tmp149_);
   double tmp151_ = tmp150_/tmp79_;
   double tmp152_ = tmp136_*tmp17_;
   double tmp153_ = tmp12_*tmp18_;
   double tmp154_ = tmp152_+tmp153_;
   double tmp155_ = (tmp154_)*(tmp8_);
   double tmp156_ = tmp136_*tmp4_;
   double tmp157_ = tmp12_*tmp19_;
   double tmp158_ = tmp156_+tmp157_;
   double tmp159_ = (tmp158_)*(tmp14_);
   double tmp160_ = tmp155_+tmp159_;
   double tmp161_ = tmp12_*tmp2_;
   double tmp162_ = tmp161_*(tmp16_);
   double tmp163_ = tmp160_+tmp162_;
   double tmp164_ = (tmp163_)*(tmp31_);
   double tmp165_ = (tmp51_)*(tmp148_);
   double tmp166_ = tmp164_-tmp165_;
   double tmp167_ = (tmp166_)/tmp79_;
   double tmp168_ = tmp67_*(tmp22_);
   double tmp169_ = tmp67_*tmp32_;
   double tmp170_ = tmp169_*(tmp31_);
   double tmp171_ = (tmp38_)*tmp168_;
   double tmp172_ = tmp170_-tmp171_;
   double tmp173_ = (tmp172_)/tmp79_;
   double tmp174_ = tmp67_*(tmp42_);
   double tmp175_ = tmp174_*(tmp31_);
   double tmp176_ = (tmp51_)*tmp168_;
   double tmp177_ = tmp175_-tmp176_;
   double tmp178_ = (tmp177_)/tmp79_;
   double tmp179_ = tmp67_*(tmp26_);
   double tmp180_ = tmp67_*tmp34_;
   double tmp181_ = tmp180_*(tmp31_);
   double tmp182_ = (tmp38_)*tmp179_;
   double tmp183_ = tmp181_-tmp182_;
   double tmp184_ = (tmp183_)/tmp79_;
   double tmp185_ = tmp67_*(tmp46_);
   double tmp186_ = tmp185_*(tmp31_);
   double tmp187_ = (tmp51_)*tmp179_;
   double tmp188_ = tmp186_-tmp187_;
   double tmp189_ = (tmp188_)/tmp79_;
   double tmp190_ = tmp67_*tmp29_;
   double tmp191_ = tmp100_*(tmp31_);
   double tmp192_ = (tmp38_)*tmp190_;
   double tmp193_ = tmp191_-tmp192_;
   double tmp194_ = (tmp193_)/tmp79_;
   double tmp195_ = tmp67_*tmp49_;
   double tmp196_ = tmp195_*(tmp31_);
   double tmp197_ = (tmp51_)*tmp190_;
   double tmp198_ = tmp196_-tmp197_;
   double tmp199_ = (tmp198_)/tmp79_;

  mVal[0] = (tmp66_)/(tmp58_)-mLocXIm;

  mCompDer[0][0] = ((tmp39_)*(tmp58_))/tmp59_;
  mCompDer[0][1] = ((tmp52_)*(tmp58_))/tmp59_;
  mCompDer[0][2] = (tmp58_)/tmp59_;
  mCompDer[0][3] = 0;
  mCompDer[0][4] = 0;
  mCompDer[0][5] = 0;
  mCompDer[0][6] = -((tmp66_)*(tmp39_))/tmp59_;
  mCompDer[0][7] = -((tmp66_)*(tmp52_))/tmp59_;
  mCompDer[0][8] = (((tmp87_)*tmp60_+(tmp99_)*tmp62_)*(tmp58_)-(tmp66_)*((tmp87_)*tmp53_+(tmp99_)*tmp55_))/tmp59_;
  mCompDer[0][9] = (((tmp122_)*tmp60_+(tmp134_)*tmp62_)*(tmp58_)-(tmp66_)*((tmp122_)*tmp53_+(tmp134_)*tmp55_))/tmp59_;
  mCompDer[0][10] = (((tmp151_)*tmp60_+(tmp167_)*tmp62_)*(tmp58_)-(tmp66_)*((tmp151_)*tmp53_+(tmp167_)*tmp55_))/tmp59_;
  mCompDer[0][11] = (((tmp173_)*tmp60_+(tmp178_)*tmp62_)*(tmp58_)-(tmp66_)*((tmp173_)*tmp53_+(tmp178_)*tmp55_))/tmp59_;
  mCompDer[0][12] = (((tmp184_)*tmp60_+(tmp189_)*tmp62_)*(tmp58_)-(tmp66_)*((tmp184_)*tmp53_+(tmp189_)*tmp55_))/tmp59_;
  mCompDer[0][13] = (((tmp194_)*tmp60_+(tmp199_)*tmp62_)*(tmp58_)-(tmp66_)*((tmp194_)*tmp53_+(tmp199_)*tmp55_))/tmp59_;
}


void cEqAppuiXDHom::ComputeValDerivHessian()
{
  ELISE_ASSERT(false,"Foncteur cEqAppuiXDHom Has no Der Sec");
}

void cEqAppuiXDHom::SetXIm(double aVal){ mLocXIm = aVal;}
void cEqAppuiXDHom::SetXTer(double aVal){ mLocXTer = aVal;}
void cEqAppuiXDHom::SetYTer(double aVal){ mLocYTer = aVal;}
void cEqAppuiXDHom::SetZTer(double aVal){ mLocZTer = aVal;}



double * cEqAppuiXDHom::AdrVarLocFromString(const std::string & aName)
{
   if (aName == "XIm") return & mLocXIm;
   if (aName == "XTer") return & mLocXTer;
   if (aName == "YTer") return & mLocYTer;
   if (aName == "ZTer") return & mLocZTer;
   return 0;
}


cElCompiledFonc::cAutoAddEntry cEqAppuiXDHom::mTheAuto("cEqAppuiXDHom",cEqAppuiXDHom::Alloc);


cElCompiledFonc *  cEqAppuiXDHom::Alloc()
{  return new cEqAppuiXDHom();
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
