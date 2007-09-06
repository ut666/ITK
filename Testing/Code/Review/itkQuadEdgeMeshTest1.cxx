/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkQuadEdgeMeshTest1.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshPolygonCell.h"
#include "itkQuadEdgeMeshLineCell.h"

int itkQuadEdgeMeshTest1( int , char* [] )
{
  std::cout << "Testing points..." << std::endl;

  typedef double PixelType;
  const unsigned int Dimension = 3;
  typedef itk::QuadEdgeMesh< PixelType, Dimension > MeshType;
  typedef MeshType::CellTraits                      CellTraits;
  typedef CellTraits::QuadEdgeType                  QEType;
  typedef MeshType::CellType                        CellType;
  typedef itk::QuadEdgeMeshPolygonCell< CellType >  QEPolygonCellType;
  typedef itk::QuadEdgeMeshLineCell< CellType >     QELineCellType;

  MeshType::Pointer  mesh = MeshType::New();

  MeshType::PointType pts[ 5 ];

  pts[ 0 ][ 0 ] = -1.0; pts[ 0 ][ 1 ] = -1.0; pts[ 0 ][ 2 ] = 0.0;
  pts[ 1 ][ 0 ] =  1.0; pts[ 1 ][ 1 ] = -1.0; pts[ 1 ][ 2 ] = 0.0;
  pts[ 2 ][ 0 ] =  1.0; pts[ 2 ][ 1 ] =  1.0; pts[ 2 ][ 2 ] = 0.0;
  pts[ 3 ][ 0 ] = -1.0; pts[ 3 ][ 1 ] =  1.0; pts[ 3 ][ 2 ] = 0.0;
  pts[ 4 ][ 0 ] =  0.0; pts[ 4 ][ 1 ] =  0.0; pts[ 4 ][ 2 ] = 1.0;

  mesh->SetPoint( 0, pts[ 0 ] );
  mesh->SetPoint( 1, pts[ 1 ] );
  mesh->SetPoint( 2, pts[ 2 ] );
  mesh->SetPoint( 3, pts[ 3 ] );
  mesh->SetPoint( 4, pts[ 4 ] );

  if( mesh->GetNumberOfPoints() != 5 )
    {
    std::cout << "Not all points added." << std::endl;
    return EXIT_FAILURE;
    }

  typedef MeshType::PointsContainer::Iterator PointsIterator;
  PointsIterator  pointIterator = mesh->GetPoints()->Begin();
  PointsIterator end = mesh->GetPoints()->End();

  int nPoints = 0;
  while( pointIterator != end )
    {
    MeshType::PointType p = pointIterator.Value();

    if( p != pts[ nPoints ] )
      {
      std::cout << "Point N. " << nPoints << " differs." << std::endl;
      return EXIT_FAILURE;
      }

    pointIterator++;
    nPoints++;
    }

  if( nPoints != 5 )
    {
    std::cout << "Iteration didn't visit all points." << std::endl;
    return EXIT_FAILURE;
    }

  // Test AddEdge failsafe
    {
    if( mesh->AddEdge( 1, 1 ) )
      {
      std::cout << "Should not be able to define an edge"
                << " with twice the same Point identifier."
                << std::endl;
      return EXIT_FAILURE;
      }
    if( mesh->AddEdge( 1, 6 ) )
      {
      std::cout << "Should not be able to define an edge"
                << " with a non existing point Id."
                << std::endl;
      return EXIT_FAILURE;
      }
    
    // create a tetahedra and one isolated point: id = 4
    int specialCells[12] =
    {  0,  1,  2,
       0,  2,  3,
       3,  1,  0,
       1,  3,  2 };

    CellType::CellAutoPointer cellpointer;
    QEPolygonCellType *poly;
    for(int i=0; i<4; i++)
      {
      poly = new QEPolygonCellType( 3 );
      cellpointer.TakeOwnership( poly );
      cellpointer->SetPointId( 0, specialCells[3*i] );
      cellpointer->SetPointId( 1, specialCells[3*i+1] );
      cellpointer->SetPointId( 2, specialCells[3*i+2] );
      mesh->SetCell( i, cellpointer );
      }

    // test origin internal
    if( mesh->AddEdge( 4, 1 ) )
      {
      std::cout << "Should not be able to define an edge"
                << " with an internal origin."
                << std::endl;
      return EXIT_FAILURE;
      }

    // test destination internal
    if( mesh->AddEdge( 1, 4 ) )
      {
      std::cout << "Should not be able to define an edge"
                << " with an internal destination."
                << std::endl;
      return EXIT_FAILURE;
      }
    }

  // test DeletePoint failsafes
    {
    mesh->DeletePoint( 1 ); // that will not delete the point
    // check it.

    }

  // test DeleteEdge failsafe
    {
    // deleteEdge( pid, pid )
    mesh->DeleteEdge( 1, 4 ); // that will not delete the edge
    // check it.
 
    // deleteEdge( QEPrimal * )
    // check with a disconnected edge

    // LightWeightDeleteEdge
    QELineCellType * qeLineCell = new QELineCellType;
    mesh->LightWeightDeleteEdge( (QEType *)NULL );
    mesh->LightWeightDeleteEdge( qeLineCell );
    mesh->LightWeightDeleteEdge( qeLineCell->GetQEGeom( ) );
    }

  std::cout << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}
