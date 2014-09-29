#include "StdAfx.h"
#include "../uti_phgrm/MICMAC/cCameraModuleOrientation.h"

/** Development based on
 CARTOSAT-1 DEM EXTRACTION CAPABILITY STUDY OVER SALON AREA
 R. Gachet & P. Fave
 */


class AffCamera
{
 public:
    AffCamera(string aFilename):a0(0),a1(1),a2(0),b0(0),b1(0),b2(1)
    {
        // Loading the GRID file
        ElAffin2D oriIntImaM2C;
        Pt2di Sz(10000,10000);
        mCamera =  new cCameraModuleOrientation(new OrientationGrille(aFilename),Sz,oriIntImaM2C);
    }

    ///
    /// \brief Tie Points (in pixel)
    ///
    std::vector<Pt2dr> vPtImg;

    ///
    /// \brief update affinity parameters
    /// \param sol unknowns matrix
    ///
    void updateParams(ElMatrix <double> const &sol)
    {
        std::cout << "Init solution: "<<std::endl;
        printParams();

        a0 += sol(0,0);
        a1 += sol(0,1);
        a2 += sol(0,2);
        b0 += sol(0,3);
        b1 += sol(0,4);
        b2 += sol(0,5);

        std::cout << "Updated solution: "<<std::endl;
        printParams();
    }

    void printParams()
    {
        std::cout << a0<<" "<<a1<<" "<<a2<<std::endl;
        std::cout << b0<<" "<<b1<<" "<<b2<<std::endl;
    }

    ElCamera* Camera() { return mCamera; }

    // the 6 parameters of affinity
    // colc = a0 + a1 * col + a2 * lig
    // ligc = b0 + b1 * col + b2 * lig
    double a0;
    double a1;
    double a2;
    double b0;
    double b1;
    double b2;

protected:

    ElCamera* mCamera;
};

//! Abstract class for shared methods
class RefineModelAbs
{
protected:
    AffCamera* master;
    AffCamera* slave;

    ///
    /// \brief Points altitude (to estimate)
    ///
    std::vector<double> vZ;
    ///
    /// \brief zMoy Ground mean altitude
    ///
    double zMoy;

    ///
    /// \brief normal matrix for least squares estimation
    ///
    ElMatrix<double> _N;
    ///
    /// \brief matrix for least squares estimation
    ///
    ElMatrix<double> _Y;

public:

    ///
    /// \brief Z estimation (iterative: 2D ground distance minimization)
    /// \param ptImgMaster tie-point from master image
    /// \param ptImgSlave  tie-point from slave image
    /// \param zIni init altitude
    /// \param dZ shift on altitude
    /// \return Z altitude of tie-point
    ///
    double getZ(Pt2dr const &ptImgMaster,
                Pt2dr const &ptImgSlave,
                double zIni,
                double dZ = 0.1) const
    {
        double z = zIni;
        Pt2dr D   = compute2DGroundDifference(ptImgMaster,ptImgSlave,z, slave);
        double d  = square_euclid(D);
        Pt2dr D1  = compute2DGroundDifference(ptImgMaster,ptImgSlave,z-dZ, slave);
        double d1 = square_euclid(D1);
        Pt2dr D2  = compute2DGroundDifference(ptImgMaster,ptImgSlave,z+dZ, slave);
        double d2 = square_euclid(D2);
        if (d1<d2)
        {
            while(d1<d)
            {
                d = d1;
                z = z-dZ;
                D1 = compute2DGroundDifference(ptImgMaster,ptImgSlave,z-dZ, slave);
                d1 = square_euclid(D1);
            }
        }
        else
        {
            while(d2<d)
            {
                d = d2;
                z = z+dZ;
                D2 = compute2DGroundDifference(ptImgMaster,ptImgSlave,z+dZ, slave);
                d2 = square_euclid(D2);
            }
        }
        return z;
    }

    ///
    /// \brief constructor (loads GRID files, tie-points and filter tie-points on 2D ground difference)
    /// \param aNameFileGridMaster Grid file for master image
    /// \param aNameFileGridSlave Grid file for slave image
    /// \param aNamefileTiePoints Tie-points file
    /// \param Zmoy ground mean altitude
    ///
    RefineModelAbs(std::string const &aNameFileGridMaster,
                   std::string const &aNameFileGridSlave,
                   std::string const &aNamefileTiePoints,
                   double Zmoy):master(NULL),slave(NULL),zMoy(Zmoy),_N(1,1,0.),_Y(1,1,0.)
    {
        // Loading the GRID file
        master = new AffCamera(aNameFileGridMaster);
        slave  = new AffCamera(aNameFileGridSlave);

        // Loading the Tie Points with altitude approximate estimation

        std::ifstream fic(aNamefileTiePoints.c_str());
        int rPts_nb = 0; //rejected points number
        while(fic.good())
        {
            Pt2dr P1,P2;
            fic >> P1.x >> P1.y >> P2.x >> P2.y;
            if (fic.good())
            {
                double z = getZ(P1,P2,zMoy);
                std::cout << "z = "<<z<<std::endl;
                Pt2dr D = compute2DGroundDifference(P1,P2,z,slave);

                if (square_euclid(D)>100.)
                {
                    rPts_nb++;
                    std::cout << "Point with 2D ground difference > 10 : "<< D.x << " " << D.y << " - rejected" << std::endl;
                }
                else
                {
                    master->vPtImg.push_back(P1);
                    slave->vPtImg.push_back(P2);
                    vZ.push_back(z);
                }
            }
        }
        std::cout << "Number of rejected points : "<< rPts_nb << std::endl;
        std::cout << "Number of tie points : "<< master->vPtImg.size() << std::endl;
    }

    ///
    /// \brief compute the difference between the Ground Points for a given Tie Point and a given set of parameters (Z and affinity)
    /// \param ptImgMaster tie-point from master image
    /// \param ptImgSlave tie-point from slave image
    /// \param aZ   ground altitude
    /// \param aA0  affinity parameter
    /// \param aA1  affinity parameter
    /// \param aA2  affinity parameter
    /// \param aB0  affinity parameter
    /// \param aB1  affinity parameter
    /// \param aB2  affinity parameter
    /// \return Pt2Dr 2D difference between ground points
    ///
    Pt2dr compute2DGroundDifference(Pt2dr const &ptImgMaster,
                                    Pt2dr const &ptImgSlave,
                                    double aZ,
                                    double aA0,
                                    double aA1,
                                    double aA2,
                                    double aB0,
                                    double aB1,
                                    double aB2)const
    {
        Pt3dr ptTerMaster = master->Camera()->ImEtProf2Terrain(ptImgMaster,aZ);
        Pt2dr ptImgSlaveC(aA0 + aA1 * ptImgSlave.x + aA2 * ptImgSlave.y,
                          aB0 + aB1 * ptImgSlave.x + aB2 * ptImgSlave.y);
        Pt3dr ptTerSlave = slave->Camera()->ImEtProf2Terrain(ptImgSlaveC,aZ);
        return Pt2dr(ptTerMaster.x - ptTerSlave.x,ptTerMaster.y - ptTerSlave.y);
    }

    Pt2dr compute2DGroundDifference(Pt2dr const &ptImgMaster,
                                    Pt2dr const &ptImgSlave,
                                    double aZ, AffCamera* cam)const
    {
        return compute2DGroundDifference(ptImgMaster,ptImgSlave,aZ,cam->a0,cam->a1,cam->a2,cam->b0,cam->b1,cam->b2);
    }

    ///
    /// \brief 2D ground distance sum for all tie points (to compute RMS)
    /// \return sum of residuals
    ///
    double sumRes()
    {
        double sumRes = 0.;
        for(size_t i=0;i<master->vPtImg.size();++i)
        {
            Pt2dr const &ptImgMaster = master->vPtImg[i];
            Pt2dr const &ptImgSlave  = slave->vPtImg[i];
            // ecart constate
            Pt2dr D = compute2DGroundDifference(ptImgMaster,ptImgSlave,vZ[i], slave);
            sumRes += square_euclid(D);
        }
        return sumRes;
    }



    ///
    /// \brief debug matrix
    /// \param mat matrix to write
    ///
    void printMatrix(ElMatrix <double> const & mat)
    {
        std::cout << "-------------------------"<<std::endl;
        for(int i=0;i<mat.Sz().x;++i)
        {
            for(int j=0;j<mat.Sz().y;++j)
                std::cout << mat(i,j) <<" ";

            std::cout << std::endl;
        }
        std::cout << "-------------------------"<<std::endl;
    }

    ///
    /// \brief check if a new iteration should be run and write result file (at the step before exiting loop)
    /// \param iniRMS rms before system solve
    /// \param numObs system number of observations
    /// \return
    ///
    bool launchNewIter(double iniRMS, int numObs)
    {
        double curRMS = std::sqrt(sumRes()/numObs);

        if (curRMS>iniRMS)
        {
            std::cout << "curRMS = "<<curRMS<<" / iniRMS = "<<iniRMS<<std::endl;
            std::cout << "No improve: end"<<std::endl;
            return false;
        }

        //ecriture dans un fichier des coefficients en vue d'affiner la grille

        std::ofstream fic("refine/refineCoef.txt");
        fic << std::setprecision(15);
        fic << slave->a0 <<" "<< slave->a1 <<" "<< slave->a2 <<" "<< slave->b0 <<" "<< slave->b1 <<" "<< slave->b2 <<" "<<std::endl;
        std::cout << "RMS_after = " << curRMS << std::endl;
        return true;
    }

    ///
    /// \brief estimates affinity parameters
    ///
    virtual void solve()=0;

    ///
    /// \brief computes the observation matrix for one iteration
    /// \return boolean stating if system is solved (need new iteration)
    ///
    virtual bool computeObservationMatrix()=0;

    virtual ~RefineModelAbs()
    {
        if (master)
            delete master;
        if (slave)
            delete slave;
    }
};

//! Implementation utilisant la suppression des inconnues auxiliaires (les Z)
class RefineModel:public RefineModelAbs
{

public:
    RefineModel(std::string const &aNameFileGridMaster,
                std::string const &aNameFileGridSlave,
                std::string const &aNamefileTiePoints,
                double Zmoy):RefineModelAbs(aNameFileGridMaster,aNameFileGridSlave,aNamefileTiePoints,Zmoy)
    {
    }

    ///
    /// \brief add an observation to system
    /// \param obs observation matrix
    /// \param p weighting
    /// \param res residual
    ///
    void addObs(const ElMatrix<double> &obs, const double p, const double res)
    {
        //construction iterative de la matrice normale
        _N += (obs.transpose()*obs)*p;  //il existe certainement une norme ou une facon plus elegante de l'ecrire ...
        //idem pour Y
        _Y += obs.transpose()*res*p;

        //on elimine la premiere inconnue
        //a00Z + SUM(a0k.Xk) = b0;
        //ai0Z + SUM(aik.Xk) = bi; ...

        double piv = _N(0,0);
        //todo si pivot trop petit ... !

        //std::cout << "piv = "<<piv<<std::endl;

        const size_t numUnk = _N.Sz().x;

        //pour les autres lignes
        for(size_t line = 1;line < numUnk; line++)
        {
            double pivotLine = _N(0,line);

            if (pivotLine == 0.) continue;

            for(size_t col = 1; col < numUnk; col++)
                _N(col,line) -= _N(col,0)*pivotLine/piv;

            //meme principe pour le vecteur Y
            _Y(0,line) -= _Y(0,0)*pivotLine/piv;
        }

        //un peu de nettoyage : 1ere ligne
        for(size_t col = 0; col < numUnk; col++)
            _N(col,0) = 0.;

        for(size_t line = 1;line < numUnk; line++)
            _N(0,line) = 0.;

        _Y(0,0) = 0.;
    }

    void solve()
    {
        _N(0,0) = 1.;

        std::cout << "Matrix _N:"<<std::endl;
        printMatrix(_N);

        std::cout << "Matrix _Y:"<<std::endl;
        printMatrix(_Y);

        ElMatrix<double> inv = gaussj(_N);
        std::cout << "Matrix inv:"<<std::endl;
        printMatrix(inv);
        ElMatrix<double> sol = inv*_Y;

        std::cout << "Matrix sol:"<<std::endl;
        printMatrix(sol);

        //std::cout << "SOL_NORM = " << sol.NormC(2) << std::endl;

        slave->updateParams(sol);

        //estimation des Z
        /*
        double dZ = 0.5, sumDZ=0.;
        for(size_t i=0;i<vPtImgMaster.size();++i)
        {
            Pt2dr const &ptImgMaster = vPtImgMaster[i];
            Pt2dr const &ptImgSlave  = vPtImgSlave[i];
            double const Z = vZ[i];

            // ecart constate
            Pt2dr D = compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0,b1,b2);

            Pt2dr vdZ  = Pt2dr(1./dZ,1./dZ)  * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z + dZ,a0,a1,a2,b0,b1,b2)-D);

            ElMatrix<double> mZ(1,2,0.);
            mZ(0,0) = vdZ.x;
            mZ(0,1) = vdZ.y;
            ElMatrix<double> NZ = mZ.transpose()*mZ;
            ElMatrix<double> resZ(1,2,0.);
            resZ(0,0) = -D.x;
            resZ(0,1) = -D.y;
            ElMatrix<double> YZ = mZ.transpose()*resZ;

            double deltaZ = YZ(0,0)/NZ(0,0);

            vZ[i] += deltaZ;
            sumDZ += deltaZ;

            std::cout << vZ[i] << std::endl;
        }
        std::cout << "SumDZ = " << sumDZ << std::endl;
        */
    }

    // compute the observation matrix for one iteration
    bool computeObservationMatrix()
    {
        // Remise a zero des matrices
        _N = ElMatrix<double>(7,7,0.);
        _Y = ElMatrix<double>(1,7,0.);

        size_t numUnk = _N.Sz().x;                       // Nombre d'inconnues
        size_t numObs = 2*master->vPtImg.size() + numUnk;  // Nombre d'observations (dont stabilisation des inconnues)

        double iniRMS = std::sqrt(sumRes()/numObs);

        std::cout << "RMS_ini = " << iniRMS << std::endl;

        double dZ =  0.5;
        double dA0 = 0.5;
        double dA1 = 0.01;
        double dA2 = 0.01;
        double dB0 = 0.5;
        double dB1 = 0.01;
        double dB2 = 0.01;

        //Ponderation
        double sigmaDelta = 1.; //m
        /*bool   weightByRes = false;
        //Ponderation stabilisation
        double sigmaTransX = 1.; //pix
        double sigmaTransY = 1.;  //pix
        double sigmaMat = 1./std::pow(0.001,2); //sans unite
        double sigmaMatA1 = 1./std::pow(0.0001,2);*/

        //pour chaque obs (ligne), y compris les eq de stabilisation
        //for( toutes les obs )

        for(size_t i=0;i<master->vPtImg.size();++i)
        {
            Pt2dr const &ptImgMaster = master->vPtImg[i];
            Pt2dr const &ptImgSlave  = slave->vPtImg[i];
            double const Z = vZ[i];

            ElMatrix<double> obs(numUnk,1,0.);
            //a remplir avec derivees partielles ...

            // ecart constate
            Pt2dr D = compute2DGroundDifference(ptImgMaster,ptImgSlave,Z, slave);

            //todo : strategie d'elimination d'observations / ou ponderation

            // estimation des derivees partielles
            double a0 = slave->a0;
            double a1 = slave->a1;
            double a2 = slave->a2;
            double b0 = slave->b0;
            double b1 = slave->b1;
            double b2 = slave->b2;

            Pt2dr vdZ  = Pt2dr(1./dZ,1./dZ)   * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z + dZ,slave)-D);
            Pt2dr vdA0 = Pt2dr(1./dA0,1./dA0) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0+dA0,a1,a2,b0,b1,b2)-D);
            Pt2dr vdA1 = Pt2dr(1./dA1,1./dA1) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1+dA1,a2,b0,b1,b2)-D);
            Pt2dr vdA2 = Pt2dr(1./dA2,1./dA2) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2+dA2,b0,b1,b2)-D);
            Pt2dr vdB0 = Pt2dr(1./dB0,1./dB0) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0+dB0,b1,b2)-D);
            Pt2dr vdB1 = Pt2dr(1./dB1,1./dB1) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0,b1+dB1,b2)-D);
            Pt2dr vdB2 = Pt2dr(1./dB2,1./dB2) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0,b1,b2+dB2)-D);

            obs(0,0) = vdZ.x;
            obs(1,0) = vdA0.x;
            obs(2,0) = vdA1.x;
            obs(3,0) = vdA2.x;
            obs(4,0) = vdB0.x;
            obs(5,0) = vdB1.x;
            obs(6,0) = vdB2.x;

            addObs(obs,sigmaDelta,0.-D.x);
            //ajout
            //if((D.x<2)&&(D.y<2)) {addObs(obs,1/std::sqrt(1+D.x*D.x),D.x);}
            //fin ajout
            obs(0,0) = vdZ.y;
            obs(1,0) = vdA0.y;
            obs(2,0) = vdA1.y;
            obs(3,0) = vdA2.y;
            obs(4,0) = vdB0.y;
            obs(5,0) = vdB1.y;
            obs(6,0) = vdB2.y;

            addObs(obs,sigmaDelta,0.-D.y);
            //ajout
            //if((D.x<2)&&(D.y<2)) {addObs(obs,1/std::sqrt(1+D.y*D.y),D.y);}
            //fin ajout
        }
        //stabilisation
        /*
        addObs(1,sigmaTransX,   0.-a0);
        addObs(2,sigmaMat,		1.-a1);
        addObs(3,sigmaMat,      0.-a2);
        addObs(4,sigmaTransY,   0.-b0);
        addObs(5,sigmaMat,      0.-b1);
        addObs(6,sigmaMat,		1.-b2);
        */
        solve();

        return launchNewIter(iniRMS, numObs);
    }


    ~RefineModel()
    {
    }
};

//! Implementation basique (sans suppression des inconnues auxiliaires)
class RefineModelBasic: public RefineModelAbs
{

public:
    RefineModelBasic(std::string const &aNameFileGridMaster,
                std::string const &aNameFileGridSlave,
                std::string const &aNamefileTiePoints,
                double Zmoy):RefineModelAbs(aNameFileGridMaster,aNameFileGridSlave,aNamefileTiePoints,Zmoy)
    {
    }

    void solve()
    {
        /*
        std::cout << "solve"<<std::endl;
        std::cout << "Matrice _N:"<<std::endl;
        printMatrix(_N);
        std::cout << "Matrice _Y:"<<std::endl;
        printMatrix(_Y);
        */
        ElMatrix<double> AtA = _N.transpose() * _N;
        //printMatrix(AtA);
        ElMatrix<double> AtB = _N.transpose() * _Y;
        //printMatrix(AtB);
        ElMatrix<double> sol = gaussj(AtA) * AtB;
        /*
        std::cout << "Matrice sol:"<<std::endl;
        printMatrix(sol);
        */
        //std::cout << "SOL_NORM = " << sol.NormC(2) << std::endl;

        slave->updateParams(sol);

        //mise a jour des Z
        for(size_t i=0;i<master->vPtImg.size();++i)
        {
            vZ[i] += sol(0,6+i);
            std::cout << vZ[i] << std::endl;
        }

    }

    //! compute the observation matrix for one iteration
    bool computeObservationMatrix()
    {
        int numObs = 2*vZ.size();
        double iniRMS = std::sqrt(sumRes()/numObs);
        std::cout << "RMS_ini = " << iniRMS << std::endl;

        double dZ  = 0.5;
        double dA0 = 0.5;
        double dA1 = 0.01;
        double dA2 = 0.01;
        double dB0 = 0.5;
        double dB1 = 0.01;
        double dB2 = 0.01;

        _N = ElMatrix<double>(6+vZ.size(),2*vZ.size()+6,0.);
        _Y = ElMatrix<double>(1,2*vZ.size()+6,0.);

        //pour chaque obs (ligne), y compris les eq de stabilisation
        //for( toutes les obs )
        for(size_t i=0;i<master->vPtImg.size();++i)
        {
            //std::cout << "i = "<<i<<std::endl;
            Pt2dr const &ptImgMaster = master->vPtImg[i];
            Pt2dr const &ptImgSlave  = slave->vPtImg[i];
            double const Z = vZ[i];

            // ecart constate
            Pt2dr D = compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,slave);

            //todo : strategie d'elimination d'observations / ou ponderation

            // estimation des derivees partielles
            double a0 = slave->a0;
            double a1 = slave->a1;
            double a2 = slave->a2;
            double b0 = slave->b0;
            double b1 = slave->b1;
            double b2 = slave->b2;

            Pt2dr vdZ  = Pt2dr(1./dZ,1./dZ)   * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z+dZ, slave) -D);
            Pt2dr vdA0 = Pt2dr(1./dA0,1./dA0) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0+dA0,a1,a2,b0,b1,b2)-D);
            Pt2dr vdA1 = Pt2dr(1./dA1,1./dA1) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1+dA1,a2,b0,b1,b2)-D);
            Pt2dr vdA2 = Pt2dr(1./dA2,1./dA2) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2+dA2,b0,b1,b2)-D);
            Pt2dr vdB0 = Pt2dr(1./dB0,1./dB0) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0+dB0,b1,b2)-D);
            Pt2dr vdB1 = Pt2dr(1./dB1,1./dB1) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0,b1+dB1,b2)-D);
            Pt2dr vdB2 = Pt2dr(1./dB2,1./dB2) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0,b1,b2+dB2)-D);

            _N(0,2*i) = vdA0.x;
            _N(1,2*i) = vdA1.x;
            _N(2,2*i) = vdA2.x;
            _N(3,2*i) = vdB0.x;
            _N(4,2*i) = vdB1.x;
            _N(5,2*i) = vdB2.x;
            _N(6+i,2*i) = vdZ.x;

            _N(0,2*i+1) = vdA0.y;
            _N(1,2*i+1) = vdA1.y;
            _N(2,2*i+1) = vdA2.y;
            _N(3,2*i+1) = vdB0.y;
            _N(4,2*i+1) = vdB1.y;
            _N(5,2*i+1) = vdB2.y;
            _N(6+i,2*i+1) = vdZ.y;

            _Y(0,2*i) = -D.x;
            _Y(0,2*i+1) = -D.y;
        }
        // Equation de stabilisation
        {
            double pdt = vZ.size();
            // A0 proche de 0
            _N(0,2*vZ.size()) = 1 * pdt;
            _Y(0,2*vZ.size()) = (0-slave->a0) * pdt;
            // A1 proche de 1
            _N(1,2*vZ.size()+1) = 1 * pdt;
            _Y(0,2*vZ.size()+1) = (1-slave->a1) * pdt;
            // A2 proche de 0
            _N(2,2*vZ.size()+2) = 1 * pdt;
            _Y(0,2*vZ.size()+2) = (0-slave->a2) * pdt;
            // B0 proche de 0
            _N(3,2*vZ.size()+3) = 1 * pdt;
            _Y(0,2*vZ.size()+3) = (0-slave->b0) * pdt;
            // B1 proche de 0
            _N(4,2*vZ.size()+4) = 1 * pdt;
            _Y(0,2*vZ.size()+4) = (0-slave->b1) * pdt;
            // B2 proche de 1
            _N(5,2*vZ.size()+5) = 1 * pdt;
            _Y(0,2*vZ.size()+5) = (1-slave->b2) * pdt;
        }
        std::cout << "before solve"<<std::endl;

        solve();

        return launchNewIter(iniRMS, numObs);
    }

    ~RefineModelBasic()
    {
    }
};

//! Implementation basique (sans suppression des inconnues auxiliaires)
class RefineModelBasicSansZ: public RefineModelAbs
{

public:
    RefineModelBasicSansZ(std::string const &aNameFileGridMaster,
                     std::string const &aNameFileGridSlave,
                     std::string const &aNamefileTiePoints,
                     double Zmoy):RefineModelAbs(aNameFileGridMaster,aNameFileGridSlave,aNamefileTiePoints,Zmoy)
    {
    }

    void solve()
    {
        /*
         std::cout << "solve"<<std::endl;
         std::cout << "Matrice _N:"<<std::endl;
         printMatrix(_N);
         std::cout << "Matrice _Y:"<<std::endl;
         printMatrix(_Y);
         */
        ElMatrix<double> AtA = _N.transpose() * _N;
        // printMatrix(AtA);

        ElMatrix<double> AtB = _N.transpose() * _Y;
        //printMatrix(AtB);
        ElMatrix<double> sol = gaussj(AtA) * AtB;
        /*
         std::cout << "Matrice sol:"<<std::endl;
         printMatrix(sol);
         */
        //std::cout << "SOL_NORM = " << sol.NormC(2) << std::endl;

        slave->updateParams(sol);

        // Z update
        for(size_t i=0;i<master->vPtImg.size();++i)
        {
            vZ[i] = getZ(master->vPtImg[i],slave->vPtImg[i],vZ[i]);
           // vZ[i] = getZ(vPtImgMaster[i],vPtImgSlave[i],vZ[i], 0.01); => legere amelioration
        }
    }

    //! compute the observation matrix for one iteration
    bool computeObservationMatrix()
    {
        int numObs = 2*vZ.size();
        double iniRMS = std::sqrt(sumRes()/numObs);
        std::cout << "RMS_ini = " << iniRMS << std::endl;

        double dA0 = 0.5;
        double dA1 = 0.01;
        double dA2 = 0.01;
        double dB0 = 0.5;
        double dB1 = 0.01;
        double dB2 = 0.01;

        _N = ElMatrix<double>(6,2*vZ.size()/*+6*/,0.);
        _Y = ElMatrix<double>(1,2*vZ.size()/*+6*/,0.);

        //pour chaque obs (ligne), y compris les eq de stabilisation
        //for( toutes les obs )
        for(size_t i=0;i<master->vPtImg.size();++i)
        {
            //std::cout << "i = "<<i<<std::endl;
            Pt2dr const &ptImgMaster = master->vPtImg[i];
            Pt2dr const &ptImgSlave  = slave->vPtImg[i];
            double const Z = vZ[i];

            // ecart constate
            Pt2dr D = compute2DGroundDifference(ptImgMaster,ptImgSlave,Z, slave);
            double ecart2 = square_euclid(D);

            double pdt = 1./sqrt(1. + ecart2);

            //todo : strategie d'elimination d'observations / ou ponderation

            // estimation des derivees partielles
            double a0 = slave->a0;
            double a1 = slave->a1;
            double a2 = slave->a2;
            double b0 = slave->b0;
            double b1 = slave->b1;
            double b2 = slave->b2;

            Pt2dr vdA0 = Pt2dr(1./dA0,1./dA0) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0+dA0,a1,a2,b0,b1,b2)-D);
            Pt2dr vdA1 = Pt2dr(1./dA1,1./dA1) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1+dA1,a2,b0,b1,b2)-D);
            Pt2dr vdA2 = Pt2dr(1./dA2,1./dA2) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2+dA2,b0,b1,b2)-D);
            Pt2dr vdB0 = Pt2dr(1./dB0,1./dB0) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0+dB0,b1,b2)-D);
            Pt2dr vdB1 = Pt2dr(1./dB1,1./dB1) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0,b1+dB1,b2)-D);
            Pt2dr vdB2 = Pt2dr(1./dB2,1./dB2) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0,b1,b2+dB2)-D);

            _N(0,2*i) = pdt * vdA0.x;
            _N(1,2*i) = pdt * vdA1.x;
            _N(2,2*i) = pdt * vdA2.x;
            _N(3,2*i) = pdt * vdB0.x;
            _N(4,2*i) = pdt * vdB1.x;
            _N(5,2*i) = pdt * vdB2.x;

            _N(0,2*i+1) = pdt * vdA0.y;
            _N(1,2*i+1) = pdt * vdA1.y;
            _N(2,2*i+1) = pdt * vdA2.y;
            _N(3,2*i+1) = pdt * vdB0.y;
            _N(4,2*i+1) = pdt * vdB1.y;
            _N(5,2*i+1) = pdt * vdB2.y;

            _Y(0,2*i)   = pdt * (0.-D.x);
            _Y(0,2*i+1) = pdt * (0.-D.y);
        }
        // Equation de stabilisation
        /*
        {
            double pdt = vZ.size()/100.;
            // A0 proche de 0
            _N(0,2*vZ.size()) = 1 * pdt;
            _Y(0,2*vZ.size()) = (0-a0) * pdt;
            // A1 proche de 1
            _N(1,2*vZ.size()+1) = 1 * pdt;
            _Y(0,2*vZ.size()+1) = (1-a1) * pdt;
            // A2 proche de 0
            _N(2,2*vZ.size()+2) = 1 * pdt;
            _Y(0,2*vZ.size()+2) = (0-a2) * pdt;
            // B0 proche de 0
            _N(3,2*vZ.size()+3) = 1 * pdt;
            _Y(0,2*vZ.size()+3) = (0-b0) * pdt;
            // B1 proche de 0
            _N(4,2*vZ.size()+4) = 1 * pdt;
            _Y(0,2*vZ.size()+4) = (0-b1) * pdt;
            // B2 proche de 1
            _N(5,2*vZ.size()+5) = 1 * pdt;
            _Y(0,2*vZ.size()+5) = (1-b2) * pdt;
        }
         */
        std::cout << "before solve"<<std::endl;

        solve();

        return launchNewIter(iniRMS, numObs);
    }

    ~RefineModelBasicSansZ()
    {
    }
};

//! Implementation basique simplifiee (uniquement la translation, pas d'estimation des 4 autres parametres)
class RefineModelTransBasic:public RefineModelAbs
{

public:
    RefineModelTransBasic(std::string const &aNameFileGridMaster,
                     std::string const &aNameFileGridSlave,
                     std::string const &aNamefileTiePoints,
                     double Zmoy):RefineModelAbs(aNameFileGridMaster,aNameFileGridSlave,aNamefileTiePoints,Zmoy)
    {
    }

    void solve()
    {
        std::cout << "solve"<<std::endl;
        std::cout << "Matrice _N:"<<std::endl;
        printMatrix(_N);
        std::cout << "Matrice _Y:"<<std::endl;
        printMatrix(_Y);

        ElMatrix<double> AtA = _N.transpose() * _N;
        printMatrix(AtA);

        ElMatrix<double> AtB = _N.transpose() * _Y;
        printMatrix(AtB);

        ElMatrix<double> sol = gaussj(AtA) * AtB;

        printMatrix(sol);

        //std::cout << "SOL_NORM = " << sol.NormC(2) << std::endl;

        std::cout << "Solution ini : "<<std::endl;
        slave->printParams();

        slave->a0 -= sol(0,0);     //pourquoi -= ???
        slave->b0 -= sol(0,1);

        std::cout << "Solution mise a jour : "<<std::endl;
        slave->printParams();

        //mise a jour des Z
        for(size_t i=0;i<master->vPtImg.size();++i)
        {
            vZ[i] -= sol(0,2+i); //pourquoi -= ???
            std::cout << vZ[i] << std::endl;
        }

    }

    //! compute the observation matrix for one iteration
    bool computeObservationMatrix()
    {
        int numObs = 2*vZ.size();
        double iniRMS = std::sqrt(sumRes()/numObs);
        std::cout << "RMS_ini = " << iniRMS << std::endl;

        double dZ = 1;//0.5;
        double dA0 = 1;//0.5;
        double dB0 = 1;//0.5;

        //Ponderation
        /*double sigmaDelta = 1./std::pow(1.,2); //m
        bool   weightByRes = false;
        //Ponderation stabilisation
        double sigmaTransX = 1./std::pow(1.,2); //pix
        double sigmaTransY = 1./std::pow(0.05,0.2);  //pix
        double sigmaMat = 1./std::pow(0.001,2); //sans unite*/

        _N = ElMatrix<double>(2+vZ.size(),2*vZ.size()+3,0.);
        _Y = ElMatrix<double>(1,2*vZ.size()+3,0.);

        //pour chaque obs (ligne), y compris les eq de stabilisation
        //for( toutes les obs )
        for(size_t i=0;i<master->vPtImg.size();++i)
        {
            std::cout << "i = "<<i<<std::endl;
            Pt2dr const &ptImgMaster = master->vPtImg[i];
            Pt2dr const &ptImgSlave  = slave->vPtImg[i];
            double const Z = vZ[i];

            // ecart constate
            Pt2dr D = compute2DGroundDifference(ptImgMaster,ptImgSlave,Z, slave);

            //todo : strategie d'elimination d'observations / ou ponderation

            // estimation des derivees partielles

            double a0 = slave->a0;
            double a1 = slave->a1;
            double a2 = slave->a2;
            double b0 = slave->b0;
            double b1 = slave->b1;
            double b2 = slave->b2;

            Pt2dr vdZ  = Pt2dr(1./dZ,1./dZ)   * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z+ dZ,slave)-D);
            Pt2dr vdA0 = Pt2dr(1./dA0,1./dA0) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0+dA0,a1,a2,b0,b1,b2)-D);
            Pt2dr vdB0 = Pt2dr(1./dB0,1./dB0) * (compute2DGroundDifference(ptImgMaster,ptImgSlave,Z,a0,a1,a2,b0+dB0,b1,b2)-D);

            _N(0,2*i) = vdA0.x;
            _N(1,2*i) = vdB0.x;
            _N(2+i,2*i) = vdZ.x;

            _N(0,2*i+1) = vdA0.y;
            _N(1,2*i+1) = vdB0.y;
            _N(2+i,2*i+1) = vdZ.y;

            _Y(0,2*i) = D.x;
            _Y(0,2*i+1) = D.y;
        }
        // Equation de stabilisation
        {
            // A0 proche de 0
            _N(0,2*vZ.size()) = 1;
            _Y(0,2*vZ.size()) = 0-slave->a0;
            // B0 proche de 0
            _N(1,2*vZ.size()+1) = 1;
            _Y(0,2*vZ.size()+1) = 0-slave->b0;
            // Moyenne alti
            double altiMoyenne = 0.;
            for(size_t i=0;i<vZ.size();++i)
            {
                _N(2+i,2*vZ.size()+2)=1;
                altiMoyenne += vZ[i];
            }
            _Y(0,2*vZ.size()+2) = zMoy-altiMoyenne/vZ.size();
        }
        std::cout << "before solve"<<std::endl;

        solve();

        return launchNewIter(iniRMS, numObs);
    }


    ~RefineModelTransBasic()
    {
    }
};

int RefineModel_main(int argc, char **argv)
{
    std::string aNameFileGridMaster; // fichier GRID image maitre
    std::string aNameFileGridSlave;  // fichier GRID image secondaire
    std::string aNameFileTiePoints;  // fichier de points de liaison
    double aZMoy;                    // the average altitude of the TiePoints

    ElInitArgMain
    (
         argc, argv,
         LArgMain() << EAMC(aNameFileGridMaster,"master image GRID")
                    << EAMC(aNameFileGridSlave,"slave image GRID")
                    << EAMC(aNameFileTiePoints,"Tie Points")
                    << EAMC(aZMoy,"average altitude of the TiePoints"),
         LArgMain()
    );

    ELISE_fp::MkDirSvp("refine");

    RefineModelBasicSansZ model(aNameFileGridMaster,aNameFileGridSlave,aNameFileTiePoints,aZMoy);

    bool ok=true;
    for(size_t iter = 0; (iter < 100) & ok; iter++)
    {
        std::cout <<"iter="<<iter<<std::endl;
        ok = model.computeObservationMatrix();
    }

    return EXIT_SUCCESS;
}




