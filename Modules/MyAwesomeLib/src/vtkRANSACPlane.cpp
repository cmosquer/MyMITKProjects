#include "vtkObjectFactory.h" //for new() macro
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
//#include "vtkCommand.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPlane.h"
#include "vtkSmartPointer.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkPlaneSource.h"

#include<set>

//#include <vtkstd/set>

#include "vtkRANSACPlane.h"



//-----------------------------------------------------------------------------
vtkRANSACPlane::vtkRANSACPlane()
{
  this->InlierThreshold = 0.6;
  this->MaxIterations = 1000;
  SetNumberOfPointsToFit(3000);
  this->GoodEnough = 50;

  this->SetNumberOfInputPorts( 1 );
  this->SetNumberOfOutputPorts( 1 );
}

void vtkRANSACPlane::SetNumberOfPointsToFit(unsigned int num)
{
  this->NumPointsToFit = num;
}

//-----------------------------------------------------------------------------
vtkRANSACPlane::~vtkRANSACPlane()
{
}


//----------------------------------------------------------------------------
void vtkRANSACPlane::AddSourceConnection(vtkAlgorithmOutput* input)
{
  this->AddInputConnection(1, input);
}


//----------------------------------------------------------------------------
void vtkRANSACPlane::RemoveAllSources()
{
  this->SetInputConnection(1, 0);
}

//----------------------------------------------------------------------------
vtkPolyData* vtkRANSACPlane::GetOutput()
{
  return vtkPolyData::SafeDownCast(this->GetOutputDataObject(0));

}

//----------------------------------------------------------------------------
int vtkRANSACPlane::FillInputPortInformation( int port, vtkInformation* info )
{
  if (!this->Superclass::FillInputPortInformation(port, info))
    {
    return 0;
    }
  if ( port == 0 )
    {
    info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet" );
    return 1;
    }
  return 0;
}


/*double* vtkRANSACPlane::GetNormal()
{
    double* n;

    n[0]=this->normal[0];

    n[1]=this->normal[1];

    n[2]=this->normal[2];

    return n;

}

double* vtkRANSACPlane::GetCenter()
{
    double* c;

    c[0]=this->center[0];

    c[1]=this->center[1];

    c[2]=this->center[2];

    return c;
}

double* vtkRANSACPlane::GetOrigin()
{
    double* c;

    c[0]=this->origin[0];

    c[1]=this->origin[1];

    c[2]=this->origin[2];

    return c;


}*/
//----------------------------------------------------------------------------
int vtkRANSACPlane::RequestData(vtkInformation *vtkNotUsed(request),
                              vtkInformationVector **inputVector,
                              vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);


  // get the input and output
  vtkPointSet *input = vtkPointSet::SafeDownCast(
                                                 inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
                                                  outInfo->Get(vtkDataObject::DATA_OBJECT()));

  
	
  //track best model
  //vtkSmartPointer<vtkPlane> bestPlane = vtkSmartPointer<vtkPlane>::New();
  this->BestPlane = vtkSmartPointer<vtkPlane>::New();
	
  //track number of inliers of best model
  unsigned int maxInliers = 0;
	
  //seed the random number gererator
  // !!! ///
  
  for(unsigned int iter = 0; iter < this->MaxIterations; iter++)
    {
    //cout << "Iteration: " << iter << endl;
    //pick NumPointsToFit random indices
    std::vector<unsigned int> randomIndices = UniqueRandomIndices(input->GetNumberOfPoints(), NumPointsToFit);
        
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    ExtractPoints(input, randomIndices, points);
        
    //find the best plane through these random points
    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    BestFitPlane(points, plane);
        
    std::vector<unsigned int> inlierIndices = this->DetermineInliers(input->GetPoints(), plane);
    //cout << "Number of inliers: " << inlierIndices.size() << endl;
    
    if(inlierIndices.size() > maxInliers)
      {
      maxInliers = inlierIndices.size();
      //bestPlane->ShallowCopy(plane);
      CopyPlane(plane, this->BestPlane);
      double n[3];
      this->BestPlane->GetNormal(n);
      //cout << "normal: " << n[0] << " " << n[1] << " " << n[2] << endl;
      }
        
    unsigned int inliers_needed=input->GetNumberOfPoints() * this->GoodEnough;
    if(inlierIndices.size() > inliers_needed ) //if GoodEnough % of the points fit the model, we can stop the search
      {
      break;
      }
    
  } //end ransac loop

 // cout << "Best plane: " << endl << "---------------" << endl;
  //cout << "Inliers: " << maxInliers << endl;

  double n[3];
  this->BestPlane->GetNormal(n);
  n[0]=-n[0];
  n[1]=-n[1];
  n[2]=-n[2];

//  cout << "normal: " << n[0] << " " << n[1] << " " << n[2] << endl;




  //output->ShallowCopy(bestPlane);
  vtkSmartPointer<vtkPlaneSource> planeSource = vtkSmartPointer<vtkPlaneSource>::New();
  planeSource->SetNormal(n);
  planeSource->SetCenter(this->BestPlane->GetOrigin());
  planeSource->SetXResolution(10);
  planeSource->SetYResolution(10);
  planeSource->Update();


    double*  orig;
    orig=planeSource->GetOrigin();

    double p1[3];
    planeSource->GetPoint1(p1);

    double p2[3];
    planeSource->GetPoint2(p2);

    double SF=40;

    p1[0]=(p1[0]-orig[0])*SF+orig[0];
    p1[1]=(p1[1]-orig[1])*SF+orig[1];
    p1[2]=(p1[2]-orig[2])*SF+orig[2];

    p2[0]=(p2[0]-orig[0])*SF+orig[0];
    p2[1]=(p2[1]-orig[1])*SF+orig[1];
    p2[2]=(p2[2]-orig[2])*SF+orig[2];

    planeSource->SetPoint1(p1);
    planeSource->SetPoint2(p2);


    planeSource->SetCenter(this->BestPlane->GetOrigin());

    planeSource->Update();
    double* n_new;

    n_new=planeSource->GetNormal();
    this->normal[0]=n_new[0];
    this->normal[1]=n_new[1];
    this->normal[2]=n_new[2];

    double* o_new;
    o_new=planeSource->GetOrigin();
    this->origin[0]=o_new[0];
    this->origin[1]=o_new[1];
    this->origin[2]=o_new[2];
	

  output->ShallowCopy(planeSource->GetOutput());
  
  return 1;
}

void vtkRANSACPlane::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

std::vector<unsigned int> vtkRANSACPlane::DetermineInliers(vtkPoints* points, vtkPlane* plane)
{
  //find the distance from every point to the plane
	
  std::vector<unsigned int> inlierIndices;
  for(unsigned int i = 0; i < points->GetNumberOfPoints(); i++)
    {
    //double distance = fabs(vgl_distance(Plane, Points[i]));
      double point[3];
      points->GetPoint(i,point);
      //double distance = plane->DistanceToPlane(p);
      double n[3];
      double o[3];
      plane->GetNormal(n);
      plane->GetOrigin(o);
      
      double distance = vtkPlane::DistanceToPlane(point, n, o);
		
    if(distance < this->InlierThreshold)
      {
      inlierIndices.push_back(i);
      }
    }
	
  return inlierIndices;
}
    
///////////////////////////////////////
////////// Helper Functions /////////////
////////////////////////////////////////
std::vector<unsigned int> UniqueRandomIndices(const unsigned int maxIndex, const unsigned int numIndices)
{
  //generate Number unique random indices from 0 to MAX

  std::vector<unsigned int> indices;
  
  //SeedRandom();
  //cannot generate more unique numbers than than the size of the set we are sampling
  if(numIndices > maxIndex)
    {
      return indices;
    }

  std::set<unsigned int> S;
  while(S.size() < numIndices)
    {
    S.insert(vtkMath::Random(0, maxIndex));
    }

  for(std::set<unsigned int>::iterator iter = S.begin(); iter != S.end(); iter++)
    {
    indices.push_back(*iter);
    }

  return indices;
}

void ExtractPoints(vtkPointSet* points, std::vector<unsigned int> indices, vtkPoints* output)
{
  for(unsigned int i = 0; i < indices.size(); i++)
    {
    double p[3];
    points->GetPoint(indices[i], p);
    output->InsertNextPoint(p[0], p[1], p[2]);
    }
}


/* allocate memory for an nrow x ncol matrix */
template<class TReal>
    TReal **create_matrix ( long nrow, long ncol )
{
  typedef TReal* TRealPointer;
  TReal **m = new TRealPointer[nrow];

  TReal* block = ( TReal* ) calloc ( nrow*ncol, sizeof ( TReal ) );
  m[0] = block;
  for ( int row = 1; row < nrow; ++row )
  {
    m[ row ] = &block[ row * ncol ];
  }
  return m;
}

/* free a TReal matrix allocated with matrix() */
template<class TReal>
    void free_matrix ( TReal **m )
{
  free ( m[0] );
  delete[] m;
}

void BestFitPlane(vtkPoints *points, vtkPlane *BestPlane)
{
  //Compute the best fit (least squares) plane through a set of points.
  vtkIdType NumPoints = points->GetNumberOfPoints();
  double dNumPoints = static_cast<double>(NumPoints);
  
  //find the center of mass of the points
  double Center[3];
  CenterOfMass(points, Center);
  //vtkstd::cout << "Center of mass: " << Center[0] << " " << Center[1] << " " << Center[2] << vtkstd::endl;
  
  //Compute sample covariance matrix
  double **a = create_matrix<double> ( 3,3 );
  a[0][0] = 0; a[0][1] = 0;  a[0][2] = 0;
  a[1][0] = 0; a[1][1] = 0;  a[1][2] = 0;
  a[2][0] = 0; a[2][1] = 0;  a[2][2] = 0;
  
  for(unsigned int pointId = 0; pointId < NumPoints; pointId++ )
  {
    double x[3];
    double xp[3];
    points->GetPoint(pointId, x);
    xp[0] = x[0] - Center[0]; 
    xp[1] = x[1] - Center[1]; 
    xp[2] = x[2] - Center[2];
    for (unsigned int i = 0; i < 3; i++)
    {
      a[0][i] += xp[0] * xp[i];
      a[1][i] += xp[1] * xp[i];
      a[2][i] += xp[2] * xp[i];
    }
  }
  
    //divide by N-1
  for(unsigned int i = 0; i < 3; i++)
  {
    a[0][i] /= dNumPoints-1;
    a[1][i] /= dNumPoints-1;
    a[2][i] /= dNumPoints-1;
  }

  // Extract eigenvectors from covariance matrix
  double **eigvec = create_matrix<double> ( 3,3 );
  
  double eigval[3];
  vtkMath::Jacobi(a,eigval,eigvec);

    //Jacobi iteration for the solution of eigenvectors/eigenvalues of a 3x3 real symmetric matrix. Square 3x3 matrix a; output eigenvalues in w; and output eigenvectors in v. Resulting eigenvalues/vectors are sorted in decreasing order; eigenvectors are normalized.
  
  //Set the plane normal to the smallest eigen vector
  BestPlane->SetNormal(eigvec[0][2], eigvec[1][2], eigvec[2][2]);
  
  //cleanup
  free_matrix(eigvec);
  free_matrix(a);
  
  //Set the plane origin to the center of mass
  BestPlane->SetOrigin(Center[0], Center[1], Center[2]);


}

void CopyPlane(vtkPlane* plane, vtkPlane* output)
{
  double n[3];
  plane->GetNormal(n);
  
  double o[3];
  plane->GetOrigin(o);
  
  output->SetNormal(n);
  output->SetOrigin(o);
  
}

void CenterOfMass(vtkPoints* points, double* center)
{
  center[0] = 0.0;
  center[1] = 0.0;
  center[2] = 0.0;
    
  for(vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
  {
    double point[3];
    points->GetPoint(i, point);
    
    center[0] += point[0];
    center[1] += point[1];
    center[2] += point[2];
  }
  
  double numberOfPoints = static_cast<double>(points->GetNumberOfPoints());
  center[0] = center[0]/numberOfPoints;
  center[1] = center[1]/numberOfPoints;
  center[2] = center[2]/numberOfPoints;
}
