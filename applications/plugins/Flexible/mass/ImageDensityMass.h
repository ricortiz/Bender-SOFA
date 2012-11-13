#ifndef SOFA_FLEXIBLE_ImageDensityMass_H
#define SOFA_FLEXIBLE_ImageDensityMass_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/behavior/Mass.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/objectmodel/Event.h>
#include <sofa/helper/vector.h>
#include <sofa/defaulttype/Vec.h>

#include <sofa/component/linearsolver/CompressedRowSparseMatrix.h>


#include <Flexible/shapeFunction/BaseShapeFunction.h>
#include <Flexible/deformationMapping/LinearJacobianBlock.inl>


#include <image/ImageTypes.h>


namespace sofa
{

namespace component
{

namespace mass
{

using sofa::component::topology::PointData;

/**
* Compute mass matrices based on a density map
* Mass is defined as a global matrix (including non diagonal terms)
* The interpolation weights are given by a BaseShapeFunction component present in the scene
* @warning the interpolation is done by a LinearJacobianBlock hard-coded in this component
* @todo find a way to describe the mass interpolation as a sofa graph with regular mappings
*/
template <class DataTypes,class ShapeFunctionTypes,class MassType>
class ImageDensityMass : public core::behavior::Mass<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE3(ImageDensityMass,DataTypes,ShapeFunctionTypes,MassType), SOFA_TEMPLATE(core::behavior::Mass,DataTypes));

    typedef core::behavior::Mass<DataTypes> Inherited;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef core::objectmodel::Data<VecCoord> DataVecCoord;
    typedef core::objectmodel::Data<VecDeriv> DataVecDeriv;


    /** @name Shape function stuff */
    //@{
    typedef core::behavior::BaseShapeFunction<ShapeFunctionTypes> BaseShapeFunction;
    typedef typename BaseShapeFunction::VReal VReal;
    typedef typename BaseShapeFunction::Gradient Gradient;
    typedef typename BaseShapeFunction::Hessian Hessian;
    typedef typename BaseShapeFunction::VRef VRef;
    typedef typename BaseShapeFunction::MaterialToSpatial MaterialToSpatial ; ///< MaterialToSpatial transformation = deformation gradient type
    typedef typename BaseShapeFunction::Coord mCoord; ///< material coordinates
    BaseShapeFunction* m_shapeFunction;        ///< the component where the weights are computed
    //@}

    /** @name Interpolation stuff */
    //@{
    typedef defaulttype::LinearJacobianBlock<DataTypes,defaulttype::Vec3Types > LinearJacobianBlock;
    typedef vector<LinearJacobianBlock> VecLinearJacobianBlock;
    //@}


    /** @name Image map stuff */
    //@{
    Data< defaulttype::ImageD > f_densityImage; ///< the density map

    typedef defaulttype::ImageLPTransform<Real> TransformType;
    Data< TransformType > f_transform;   ///< transform of the density map
    //@}


    /** @name Mass stuff */
    //@{
    //typedef helper::vector<MassType> VecMass;

    /// store the mass matrices (size of elements if used)
    //Data< VecMass > f_masses;


    typedef linearsolver::CompressedRowSparseMatrix<MassType> MassMatrix; ///< the global mass matrix type
    MassMatrix m_massMatrix; ///< the global mass matrix

    Data< bool > f_isLumped;

    //@}

protected:

    ImageDensityMass()
        : f_densityImage( initData(&f_densityImage, "densityImage", "A density map") )
        , f_transform( initData( &f_transform, TransformType(), "transform", "The density map transform" ) )
        , f_isLumped( initData( &f_isLumped, false, "isLumped", "Should the mass matrix be lumped?" ) )
    {}

    virtual ~ImageDensityMass();

public:


    void clear();

    virtual void reinit();
    virtual void init();



    // -- Mass interface
    void addMDx(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecDeriv& f, const DataVecDeriv& dx, double factor);

    void accFromF(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecDeriv& a, const DataVecDeriv& f);

    void addForce(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v);

    double getKineticEnergy(const core::MechanicalParams* mparams /* PARAMS FIRST */, const DataVecDeriv& v) const;  ///< vMv/2 using dof->getV()

    double getPotentialEnergy(const core::MechanicalParams* mparams /* PARAMS FIRST */, const DataVecCoord& x) const;   ///< Mgx potential in a uniform gravity field, null at origin

    void addGravityToV(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecDeriv& d_v);

    /// Add Mass contribution to global Matrix assembling
    void addMToMatrix(const core::MechanicalParams *mparams /* PARAMS FIRST */, const sofa::core::behavior::MultiMatrixAccessor* matrix);


    bool isDiagonal() { return false; }

    void draw(const core::visual::VisualParams* vparams);


    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const ImageDensityMass<DataTypes, ShapeFunctionTypes, MassType>* = NULL)
    {
        return DataTypes::Name()+std::string(",")+ShapeFunctionTypes::Name()/*+","+MassType::Name()*/;
    }

protected:

    /// \returns the cross contribution (J1^T.voxelMass.J0) to the dof mass
    /// notNull is set to true iff one entry of the returned matrix is not null
    MassType J1tmJ0( /*const*/ LinearJacobianBlock& J0, /*const*/ LinearJacobianBlock& J1, Real voxelMass, bool& notNull );
};



#if defined(SOFA_EXTERN_TEMPLATE) && !defined(SOFA_FLEXIBLE_ImageDensityMass_CPP)
#ifndef SOFA_FLOAT
extern template class SOFA_BASE_MECHANICS_API ImageDensityMass<defaulttype::Vec3dTypes,core::behavior::ShapeFunction3d,defaulttype::Mat3x3d>; // volume FEM (tetra, hexa)
extern template class SOFA_BASE_MECHANICS_API ImageDensityMass<defaulttype::Vec3dTypes,core::behavior::ShapeFunction2d,defaulttype::Mat3x3d>; // surface FEM (triangles, quads)
extern template class SOFA_BASE_MECHANICS_API ImageDensityMass<defaulttype::Affine3dTypes,core::behavior::ShapeFunction3d,defaulttype::Affine3dMass>; // affine frames
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_BASE_MECHANICS_API ImageDensityMass<defaulttype::Vec3fTypes,core::behavior::ShapeFunction3f,defaulttype::Mat3x3f>;
extern template class SOFA_BASE_MECHANICS_API ImageDensityMass<defaulttype::Vec3fTypes,core::behavior::ShapeFunction2f,defaulttype::Mat3x3f>;
extern template class SOFA_BASE_MECHANICS_API ImageDensityMass<defaulttype::Affine3fTypes,core::behavior::ShapeFunction3f,defaulttype::Affine3fMass>;
#endif
#endif

} // namespace mass

} // namespace component

} // namespace sofa

#endif // SOFA_FLEXIBLE_ImageDensityMass_H