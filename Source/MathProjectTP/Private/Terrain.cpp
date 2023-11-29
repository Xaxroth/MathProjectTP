
#include "Terrain.h"
#include "ProceduralMeshComponent.h"

ATerrain::ATerrain()
{
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Procedural Mesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
}

void ATerrain::BeginPlay()
{
	Super::BeginPlay();

	RandomGeneratedNumber = FMath::FRandRange(MinValue, MaxValue);

	CreateVertices();
	CreateTriangles();

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// NOISE //
// A method that creates vertices in the world space, the position of which is determined by PerlinNoise2D which makes a "grid" of vertices that can later be drawn as a dynamic mesh
// by drawing triangles between each point. In order to make the terrain generation random for each time the game is started, the noise scale has a randomly generated number attached to it.
// The z float disturbed by the PerlinNoise is what eventually creates the variations on the z-axis on the grid.

void ATerrain::CreateVertices() 
{
	for (int x = 0; x <= XSize; x++)
	{
		for (int y = 0; y <= YSize; y++)
		{
			float z = FMath::PerlinNoise2D(FVector2D(x * NoiseScale + RandomGeneratedNumber, y * NoiseScale + RandomGeneratedNumber)) * PerlinNoiseMultiplier;
			Vertices.Add(FVector(x * Scale, y * Scale, z));
			UV0.Add(FVector2D(x * Scale, y * Scale));
		}
	}
}

// Drawing the triangles between each vertex point. Forms a square which consists of two triangles, both of which have three vertex points each.

void ATerrain::CreateTriangles() 
{
	int Vertex = 0;

	for (int x = 0; x < XSize; ++x)
	{
		for (int y = 0; y < YSize; ++y)
		{
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);

			++Vertex;
		}
		++Vertex;
	}

}
