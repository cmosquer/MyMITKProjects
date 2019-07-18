//Because vtkPlane is strange, we will return as output a PolyData and provide Normal and Origin accessors.


#ifndef __vtkRANSACPlane_h
#define __vtkRANSACPlane_h

#include "vtkPolyDataAlgorithm.h" //superclass

//#include "vtkstd/vector"

#include <vector>
#include <vtkPlaneSource.h>
#include <vtkPlane.h>

#include <MyAwesomeLibExports.h>

//#include <vtkVector.h>

/*class vtkPolyData;
class vtkPlane;
class vtkPoints;

class vtkInformation;
class vtkInformationVector;*/

class MyAwesomeLib_EXPORT vtkRANSACPlane : public vtkPolyDataAlgorithm
{
 public:
   //static vtkRANSACPlane* New();
   void vtkTypeRevisionMacro(vtkRANSACPlane, vtkPolyDataAlgorithm);
  void PrintSelf(ostream &os, vtkIndent indent);

  // Description:
  // Specify the source object. This is the object that will be moved during the transformation.
  vtkPolyData *GetSource();

  // Description:
  // Specify the target object. This is the object that will stay in place.
  vtkPolyData *GetTarget();

  void AddSourceConnection(vtkAlgorithmOutput* input);
  void RemoveAllSources();

  void SetNumberOfPointsToFit(unsigned int num);

  vtkPolyData* GetOutput();
  
  vtkSetMacro(InlierThreshold, double);
  vtkGetMacro(InlierThreshold, double);
      
  vtkSetMacro(MaxIterations, double);
  vtkGetMacro(MaxIterations, double);


  vtkSetMacro(GoodEnough, double);
  vtkGetMacro(GoodEnough, double);

  vtkSetMacro(NumPointsToFit, double);
  vtkGetMacro(NumPointsToFit, double);



  vtkSmartPointer<vtkPlane> BestPlane;

    vtkRANSACPlane();
   ~vtkRANSACPlane();

  int FillInputPortInformation( int port, vtkInformation* info );
    
  // Generate output
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  /*double* GetNormal();
  double* GetCenter();
  double* GetOrigin();
  */


  double normal[3];
  double origin[3];
  double center[3];



 private:

   double InlierThreshold; //how close to the plane a point must be to be considered an inlier
   unsigned int MaxIterations; //how many times to pick points and fit a plane
   double GoodEnough; //The percentage of the data that is fit to consider the model "good enough"
   unsigned int NumPointsToFit; //how many points to pick and fit a plane to in each iteration

   std::vector<unsigned int> DetermineInliers(vtkPoints* points, vtkPlane* plane);

};

////////// Helper Functions /////////////
std::vector<unsigned int> UniqueRandomIndices(const unsigned int maxIndex, const unsigned int numIndices);
//vtkstd::vector<unsigned int> UniqueRandomIndices(const unsigned int maxIndex, const unsigned int numIndices);
void ExtractPoints(vtkPointSet* polydata, std::vector<unsigned int> indices, vtkPoints* output);
void BestFitPlane(vtkPoints *points, vtkPlane *BestPlane);
void CopyPlane(vtkPlane* plane, vtkPlane* output);
void CenterOfMass(vtkPoints* points, double* center);
    
#endif
