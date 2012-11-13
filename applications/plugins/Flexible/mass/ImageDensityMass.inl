#ifndef SOFA_FLEXIBLE_ImageDensityMass_INL
#define SOFA_FLEXIBLE_ImageDensityMass_INL

#include "ImageDensityMass.h"
#include <sofa/core/behavior/Mass.inl>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/defaulttype/DataTypeInfo.h>
//#include <sofa/component/mass/AddMToMatrixFunctor.h>


#include <Flexible/types/AffineTypes.h>

#include <sofa/helper/gl/template.h>

namespace sofa
{

namespace component
{

namespace mass
{

using namespace	sofa::component::topology;
using namespace core::topology;
using namespace sofa::defaulttype;
using namespace sofa::core::behavior;








template < class DataTypes, class ShapeFunctionTypes, class MassType >
ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::~ImageDensityMass()
{
}


///////////////////////////////////////////


template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::clear()
{
    m_massMatrix.clear();
}




//////////////////////////////////



template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::init()
{
    Inherited::init();

    reinit();

    //std::cerr<<m_massMatrix<<std::endl;
}



template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::reinit()
{
    // get the shape function component
    this->getContext()->get( m_shapeFunction, core::objectmodel::BaseContext::Local );
    if( !m_shapeFunction )
    {
        serr << "ShapeFunction<"<<ShapeFunctionTypes::Name()<<"> component not found" << sendl;
        return;
    }

    const VecCoord& DOFX0 = *this->mstate->getX0();

    // eventually resize and always clear
    m_massMatrix.resizeBloc( DOFX0.size(), DOFX0.size() ); // one block per dof


    const TransformType& transform = f_transform.getValue();

    // get the density image
    const CImg<double>& densityImage = f_densityImage.getValue().getCImg(0);

    // for each density voxel
    cimg_forXYZ( densityImage, x, y, z )
    {
        // get the voxel density from the image
        double voxelDensity = densityImage( x, y, z );

        if( voxelDensity > 0 )
        {
            // the voxel position in space
            mCoord voxelPos = transform.fromImage( typename TransformType::Coord( x, y, z ) );

            // compute interpolation points/weights
            VRef controlPoints;  ///< The cp indices. controlPoints[j] is the index of the j-th parent influencing child.
            VReal weights; ///< The cp weights. weights[j] is the weight of the j-th parent influencing child.
            MaterialToSpatial M; // what is that?!!
            //VGradient gradients;
            //VHessian hessians;
            m_shapeFunction->computeShapeFunction( voxelPos, M, controlPoints, weights/*, gradients, hessians*/ );

            // get the voxel density
            double voxelVolume = transform.getScale()[0] * transform.getScale()[1] * transform.getScale()[2];
            double voxelMass = voxelDensity * voxelVolume;

            // check the real number of control points
            unsigned nbControlPoints = 0;
            for( unsigned k=0; k<controlPoints.size() && weights[k]>0 ; ++k,++nbControlPoints );

            // precompute the interpolation matrix for each control points
            VecLinearJacobianBlock linearJacobians;
            linearJacobians.resize( nbControlPoints );
            for( unsigned k=0; k<nbControlPoints ; k++ )
                linearJacobians[k].init( DOFX0[controlPoints[k]], voxelPos, voxelPos, typename LinearJacobianBlock::MaterialToSpatial(), weights[k], Gradient(), Hessian() );

            MassType JltmJk;
            bool notNull;

            // for each control point influencing the voxel
            for( unsigned k=0; k<nbControlPoints ; k++ )
            {
                const unsigned cp_k = controlPoints[k];

                // influence of the same dof with itself
                JltmJk = J1tmJ0( linearJacobians[k], linearJacobians[k], voxelMass, notNull );

                if( notNull )
                {
                    MassType& Mkk = *m_massMatrix.wbloc(cp_k,cp_k,true);
                    Mkk += JltmJk;
                }


                for( unsigned l=k+1; l<nbControlPoints ; l++ )
                {
                    const unsigned cp_l = controlPoints[l];

                    // influence of 2 different dofs, ie non-diagonal terms
                    JltmJk = J1tmJ0( linearJacobians[k], linearJacobians[l], voxelMass, notNull );

                    if( notNull )
                    {
                        if( f_isLumped.getValue() )
                        {
                            // sum to the diagonal term on the same line
                            MassType& Mkk = *m_massMatrix.wbloc(cp_k,cp_k,true);
                            Mkk += JltmJk;

                            MassType& Mll = *m_massMatrix.wbloc(cp_l,cp_l,true);
                            Mll += JltmJk.transposed();
                        }
                        else
                        {
                            MassType& Mkl = *m_massMatrix.wbloc(cp_k,cp_l,true);
                            Mkl += JltmJk;

                            MassType& Mlk = *m_massMatrix.wbloc(cp_l,cp_k,true);
                            Mlk += JltmJk.transposed();
                        }
                    }
                }
            }
        }
    }
}




template < class DataTypes, class ShapeFunctionTypes, class MassType >
MassType ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::J1tmJ0( LinearJacobianBlock& J0, LinearJacobianBlock& J1, Real voxelMass, bool& notNull )
{
    MassType M;
    for( int w=0 ; w<DataTypes::deriv_total_size ; ++w ) // for all cinematic dof
    {
        Deriv m;
        Deriv acc; acc[w] = 1; // create a pseudo acceleration, to compute JtJ line by line
        defaulttype::Vec3Types::Deriv force;

        // map the pseudo acceleration from the dof level to the voxel level
        J0.addmult( force, acc );

        // compute a pseudo-force at voxel level f=ma
        force *= voxelMass;

        // bring back the pseudo-force at dof level
        J1.addMultTranspose( m , force );

        for( int v=0 ; v<DataTypes::deriv_total_size ; ++v ) // for all cinematic dof
            M[w][v] = m[v];
    }

    // check if M is not null
    notNull = false;
    for( int w=0 ; w<DataTypes::deriv_total_size && !notNull ; ++w )
        for( int v=0 ; v<DataTypes::deriv_total_size && !notNull ; ++v )
            if( M[w][v] !=0 ) notNull = true; // TODO should the equality have a threshold?

    return M;
}





///////////////////////////////////////////












// -- Mass interface
template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::addMDx( const core::MechanicalParams* /* PARAMS FIRST */, DataVecDeriv& res, const DataVecDeriv& dx, double factor )
{
    VecDeriv& _res = *res.beginEdit();

    if( factor == 1.0 )
    {
        const VecDeriv& _dx = dx.getValue();
        m_massMatrix.addMul( _res, _dx );
    }
    else
    {
        VecDeriv _dx = dx.getValue();

        for( unsigned int i=0 ; i<_dx.size() ; i++ )
        {
            _dx[i] *= factor;
        }

        m_massMatrix.addMul( _res, _dx );
    }
}

template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::accFromF(const core::MechanicalParams* /* PARAMS FIRST */, DataVecDeriv& , const DataVecDeriv&)
{
    serr<<"void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::accFromF(VecDeriv& a, const VecDeriv& f) not yet implemented (need the matrix assembly and inversion)"<<sendl;
}

template < class DataTypes, class ShapeFunctionTypes, class MassType >
double ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::getKineticEnergy( const core::MechanicalParams* /* PARAMS FIRST */, const DataVecDeriv& ) const
{
    serr<<"void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::getKineticEnergy not yet implemented"<<sendl;
    return 0;
}

template < class DataTypes, class ShapeFunctionTypes, class MassType >
double ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::getPotentialEnergy( const core::MechanicalParams* /* PARAMS FIRST */, const DataVecCoord& ) const
{
    serr<<"void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::getPotentialEnergy not yet implemented"<<sendl;
    return 0;
}



template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::addGravityToV(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecDeriv& d_v)
{
    if(mparams)
    {
        VecDeriv& v = *d_v.beginEdit();

        // gravity
        Vec3d g ( this->getContext()->getGravity() * (mparams->dt()) );
        Deriv theGravity;
        DataTypes::set ( theGravity, g[0], g[1], g[2]);
        Deriv hg = theGravity * (mparams->dt());

        // add weight force
        for (unsigned int i=0; i<v.size(); i++)
        {
            v[i] += hg;
        }
        d_v.endEdit();
    }
}


template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::addForce(const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, DataVecDeriv& f, const DataVecCoord& /*x*/, const DataVecDeriv& /*v*/)
{
    //if gravity was added separately (in solver's "solve" method), then nothing to do here
    if(this->m_separateGravity.getValue()) return;

    VecDeriv& _f = *f.beginEdit();

    // gravity
    Vec3d g ( this->getContext()->getGravity() );
    Deriv theGravity;
    DataTypes::set ( theGravity, g[0], g[1], g[2]);


    // add weight
    m_massMatrix.addMul_by_line( _f, theGravity );

}

template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::addMToMatrix(const core::MechanicalParams */*mparams*/ /* PARAMS FIRST */, const sofa::core::behavior::MultiMatrixAccessor* /*matrix*/)
{
    serr<<"void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::addMToMatrix not yet implemented"<<sendl;
}


///////////////////////

template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::draw(const core::visual::VisualParams* /*vparams*/)
{

}



} // namespace mass

} // namespace component

} // namespace sofa

#endif // SOFA_FLEXIBLE_ImageDensityMass_INL