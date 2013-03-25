cmake_minimum_required(VERSION 2.8)

project("SofaBoundaryCondition")

include(${SOFA_CMAKE_DIR}/pre.cmake)

set(HEADER_FILES

    initBoundaryCondition.h 
    forcefield/BuoyantForceField.h 
    forcefield/BuoyantForceField.inl 
    forcefield/ConicalForceField.h 
    forcefield/ConicalForceField.inl 
    forcefield/ConstantForceField.h 
    forcefield/ConstantForceField.inl 
    forcefield/EdgePressureForceField.h 
    forcefield/EdgePressureForceField.inl 
    forcefield/EllipsoidForceField.h 
    forcefield/EllipsoidForceField.inl 
    forcefield/LinearForceField.h 
    forcefield/LinearForceField.inl 
    forcefield/OscillatingTorsionPressureForceField.h 
    forcefield/OscillatingTorsionPressureForceField.inl 
    forcefield/PlaneForceField.h 
    forcefield/PlaneForceField.inl 
    forcefield/SphereForceField.h 
    forcefield/SphereForceField.inl 
    forcefield/SurfacePressureForceField.h 
    forcefield/SurfacePressureForceField.inl 
    forcefield/TaitSurfacePressureForceField.h 
    forcefield/TaitSurfacePressureForceField.inl 
    forcefield/TrianglePressureForceField.h 
    forcefield/TrianglePressureForceField.inl 
    forcefield/VaccumSphereForceField.h 
    forcefield/VaccumSphereForceField.inl 
    projectiveconstraintset/FixedConstraint.h 
    projectiveconstraintset/FixedConstraint.inl 
    projectiveconstraintset/FixedPlaneConstraint.h 
    projectiveconstraintset/FixedPlaneConstraint.inl 
    projectiveconstraintset/FixedRotationConstraint.h 
    projectiveconstraintset/FixedRotationConstraint.inl 
    projectiveconstraintset/FixedTranslationConstraint.h 
    projectiveconstraintset/FixedTranslationConstraint.inl 
    projectiveconstraintset/HermiteSplineConstraint.h 
    projectiveconstraintset/HermiteSplineConstraint.inl 
    projectiveconstraintset/LinearMovementConstraint.h 
    projectiveconstraintset/LinearMovementConstraint.inl 
    projectiveconstraintset/LinearVelocityConstraint.h 
    projectiveconstraintset/LinearVelocityConstraint.inl 
    projectiveconstraintset/OscillatorConstraint.h 
    projectiveconstraintset/OscillatorConstraint.inl 
    projectiveconstraintset/ParabolicConstraint.h 
    projectiveconstraintset/ParabolicConstraint.inl 
    projectiveconstraintset/PartialFixedConstraint.h 
    projectiveconstraintset/PartialFixedConstraint.inl 
    projectiveconstraintset/PartialLinearMovementConstraint.h 
    projectiveconstraintset/PartialLinearMovementConstraint.inl 
    projectiveconstraintset/PointConstraint.h 
    projectiveconstraintset/PointConstraint.inl 
    projectiveconstraintset/PositionBasedDynamicsConstraint.h 
    projectiveconstraintset/PositionBasedDynamicsConstraint.inl 
    projectiveconstraintset/SkeletalMotionConstraint.h 
    projectiveconstraintset/SkeletalMotionConstraint.inl
    )
    
set(SOURCE_FILES

    initBoundaryCondition.cpp 
    forcefield/BuoyantForceField.cpp 
    forcefield/ConicalForceField.cpp 
    forcefield/ConstantForceField.cpp 
    forcefield/EdgePressureForceField.cpp 
    forcefield/EllipsoidForceField.cpp 
    forcefield/LinearForceField.cpp 
    forcefield/OscillatingTorsionPressureForceField.cpp 
    forcefield/PlaneForceField.cpp 
    forcefield/SphereForceField.cpp 
    forcefield/SurfacePressureForceField.cpp 
    forcefield/TaitSurfacePressureForceField.cpp 
    forcefield/TrianglePressureForceField.cpp 
    forcefield/VaccumSphereForceField.cpp 
    projectiveconstraintset/FixedConstraint.cpp 
    projectiveconstraintset/FixedPlaneConstraint.cpp 
    projectiveconstraintset/FixedRotationConstraint.cpp 
    projectiveconstraintset/FixedTranslationConstraint.cpp 
    projectiveconstraintset/HermiteSplineConstraint.cpp 
    projectiveconstraintset/LinearMovementConstraint.cpp 
    projectiveconstraintset/LinearVelocityConstraint.cpp 
    projectiveconstraintset/OscillatorConstraint.cpp 
    projectiveconstraintset/ParabolicConstraint.cpp 
    projectiveconstraintset/PartialFixedConstraint.cpp 
    projectiveconstraintset/PartialLinearMovementConstraint.cpp 
    projectiveconstraintset/PointConstraint.cpp 
    projectiveconstraintset/PositionBasedDynamicsConstraint.cpp 
    projectiveconstraintset/SkeletalMotionConstraint.cpp
    )
    
if(SOFA_HAVE_EIGEN2)
    list(APPEND HEADER_FILES "projectiveconstraintset/ProjectToLineConstraint.h")
    list(APPEND HEADER_FILES "projectiveconstraintset/ProjectToLineConstraint.inl")
    list(APPEND HEADER_FILES "projectiveconstraintset/ProjectToPlaneConstraint.h")
    list(APPEND HEADER_FILES "projectiveconstraintset/ProjectToPlaneConstraint.inl")
    list(APPEND HEADER_FILES "projectiveconstraintset/ProjectToPointConstraint.h")
    list(APPEND HEADER_FILES "projectiveconstraintset/ProjectToPointConstraint.inl")
    list(APPEND HEADER_FILES "projectiveconstraintset/ProjectDirectionConstraint.h")
    list(APPEND HEADER_FILES "projectiveconstraintset/ProjectDirectionConstraint.inl")

    list(APPEND SOURCE_FILES "projectiveconstraintset/ProjectToLineConstraint.cpp")
    list(APPEND SOURCE_FILES "projectiveconstraintset/ProjectToPlaneConstraint.cpp")
    list(APPEND SOURCE_FILES "projectiveconstraintset/ProjectToPointConstraint.cpp")
    list(APPEND SOURCE_FILES "projectiveconstraintset/ProjectDirectionConstraint.cpp")
endif()    
    
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} SofaBaseTopology SofaEigen2Solver)
    
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "${GLOBAL_DEFINES};SOFA_BUILD_BOUNDARY_CONDITION")
    
include(${SOFA_CMAKE_DIR}/post.cmake)