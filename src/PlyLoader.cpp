
#include "PlyLoader.h"
#include "rply.h"  // ply parsing library, courtesy of diego

#include <assert.h>
#include <string>
#include <float.h>

#include "Types.h"

#include "Mesh.h"

typedef unsigned int UINT;

struct Context
{
    Simpleton::PlyMesh* pMesh;
    UINT pStripCache[3];
    UINT  nStripCounter; // used to track cuts
    UINT* pNextFace;     // for filling in tri-strips
};


static Context* GetPlyContext( p_ply_argument  argument )
{
    Context* pContext;
    ply_get_argument_user_data( argument, (void**) &pContext, 0 );
    return pContext;
};

static int VertexCallback( p_ply_argument  argument )
{
    // figure out which vertex component this is
    long nVertexComponent;
    Context* pContext;
    ply_get_argument_user_data( argument, (void**) &pContext, &nVertexComponent );

    // figure out this vertex's index
    long nIndex;
    ply_get_argument_element( argument, NULL, &nIndex );
    
    // get the value
    float fValue = (float) ply_get_argument_value( argument );
    pContext->pMesh->pPositions[ nIndex ][ nVertexComponent ] = fValue;

    // update AABB
    if( fValue < pContext->pMesh->bbMin[nVertexComponent] )
        pContext->pMesh->bbMin[nVertexComponent] = fValue;
    if( fValue > pContext->pMesh->bbMax[nVertexComponent] )
        pContext->pMesh->bbMax[nVertexComponent] = fValue;

    return 1;
}
static int VertexNormalCallback( p_ply_argument  argument )
{
    // figure out which vertex component this is
    long nVertexComponent;
    Context* pContext;
    ply_get_argument_user_data( argument, (void**) &pContext, &nVertexComponent );

    // figure out this vertex's index
    long nIndex;
    ply_get_argument_element( argument, NULL, &nIndex );
    
    // get the value
    double value = ply_get_argument_value( argument );
    
    // write it
    pContext->pMesh->pNormals[ nIndex ][ nVertexComponent ] = (float) value;

    return 1;
}
static int VertexUVCallback( p_ply_argument  argument )
{
    // figure out which vertex component this is
    long nVertexComponent;
    Context* pContext;
    ply_get_argument_user_data( argument, (void**) &pContext, &nVertexComponent );

    // figure out this vertex's index
    long nIndex;
    ply_get_argument_element( argument, NULL, &nIndex );
    
    // get the value
    double value = ply_get_argument_value( argument );
    
    // write it
    pContext->pMesh->pUVs[ nIndex ][ nVertexComponent ] = (float) value;

    return 1;
}

static int TriStripCallback( p_ply_argument argument )
{
    long length, value_index;
    ply_get_argument_property(argument, NULL, &length, &value_index);

    Context* pContext = GetPlyContext(argument);
    if( value_index == -1 )
    {
        // beginning of list.  We need to allocate the list now, because we've only just now learned the strip length
        // we'll assume one face per strip index, which will be a bit of an overestimate if there are cuts
        pContext->pMesh->pVertexIndices = new UINT[ 3*(length-2) ];
        pContext->pNextFace = pContext->pMesh->pVertexIndices;
        pContext->nStripCounter = 0;
    }
    else
    {
        UINT nIndex = (UINT) ply_get_argument_value( argument );
        
        // reset counter on a strip cut
        if( nIndex == 0xffffffff )
        {
            pContext->nStripCounter = 0;
        }
        else
        {
            // non-cut, put the index into the cache
            UINT nStoreLoc = pContext->nStripCounter % 3;
            pContext->pStripCache[ nStoreLoc ] = nIndex;
            pContext->nStripCounter++;

            // start assembling triangles once we have enough indices
            if( pContext->nStripCounter >= 3 )
            {
                UINT i0 = pContext->pStripCache[ (nStoreLoc + 2) % 3 ];
                UINT i1 = pContext->pStripCache[ (nStoreLoc + 1) % 3 ];
                UINT i2 = pContext->pStripCache[ nStoreLoc ];

                // flip winding on odd faces
                if( pContext->nStripCounter & 1 )
                {
                    pContext->pNextFace[0] = i0;
                    pContext->pNextFace[1] = i2;
                    pContext->pNextFace[2] = i1;
                }
                else
                {
                    pContext->pNextFace[0] = i0;
                    pContext->pNextFace[1] = i1;
                    pContext->pNextFace[2] = i2;
                }
                
                pContext->pNextFace += 3;
            }
           
        }
    }

    return 1;
}

static int FaceListCallback( p_ply_argument argument )
{
    long length, nVertIndex;
    ply_get_argument_property(argument, NULL, &length, &nVertIndex);

    if( length != 3 )
        return 0;    // not a triangle!  We won't work!

    // figure out the face index
    long nFaceIndex;
    ply_get_argument_element( argument, NULL, &nFaceIndex );
  
    //-1 means beginning of list
    if( nVertIndex != -1 )
    {
        Context* pContext = GetPlyContext(argument);
        pContext->pMesh->pVertexIndices[3*nFaceIndex + nVertIndex] = (UINT) ply_get_argument_value( argument );
    }
    
    return 1;
}

static int FaceColorCallback( p_ply_argument  argument )
{
    // figure out which component this is
    long nColorComponent;
    ply_get_argument_user_data( argument, 0, &nColorComponent );

    // figure out the face index
    long nIndex;
    ply_get_argument_element( argument, NULL, &nIndex );
    
    // get the value
    double value = ply_get_argument_value( argument );
    UINT nValue = (UINT) value;

    Context* pContext = GetPlyContext(argument);

    UINT nMask = 0xff << 8*( nColorComponent+1 );
    pContext->pMesh->pFaceColors[ nIndex ].Channels[nColorComponent] = static_cast<uint8>(nValue);

    return 1;
}


static int VertexColorCallback( p_ply_argument  argument )
{
    // figure out which component this is
    long nColorComponent;
    ply_get_argument_user_data( argument, 0, &nColorComponent );

    // figure out the face index
    long nIndex;
    ply_get_argument_element( argument, NULL, &nIndex );
    
    // get the value
    double value = ply_get_argument_value( argument );
    UINT nValue = (UINT) value;

    Context* pContext = GetPlyContext(argument);

    UINT nMask = 0xff << 8*( nColorComponent+1 );
    pContext->pMesh->pVertexColors[ nIndex ].Channels[nColorComponent] = static_cast<uint8>(nValue);

    return 1;
}


// convert the mesh vertices into the form that Josh likes
static void PostProcessPositions( Simpleton::PlyMesh* pMesh )
{    
    // push vertices up so that lowest point of mesh is at y=0
    // and x,z are centered at 0
    float dy = -pMesh->bbMin[1];
    float dx = -( pMesh->bbMin[0] + pMesh->bbMax[0] ) * 0.5f;
    float dz = -( pMesh->bbMin[2] + pMesh->bbMax[2] ) * 0.5f;

    // find scale factor for normalization (size on largest axis)
    float fSizeScale=0;
    UINT nMaxComp=0;
    for( UINT i=0; i<3; i++ )
    {
        float fSize = pMesh->bbMax[i]-pMesh->bbMin[i];
        if( fSize > fSizeScale )
        {
            nMaxComp = i;
            fSizeScale = fSize;
        }
    }

    fSizeScale = 1.0f / fSizeScale;

    for( UINT i=0; i<pMesh->nVertices; i++ )
    {
        pMesh->pPositions[i][0] += dx;
        pMesh->pPositions[i][0] *= fSizeScale;

        pMesh->pPositions[i][1] += dy;
        pMesh->pPositions[i][1] *= fSizeScale;

        pMesh->pPositions[i][2] += dz;
        pMesh->pPositions[i][2] *= fSizeScale;
    }

    // fix the aabb
    pMesh->bbMin[0] += dx;
    pMesh->bbMax[0] += dx;
    pMesh->bbMin[1] = 0;
    pMesh->bbMax[1] += dy;
    pMesh->bbMin[2] += dz;
    pMesh->bbMin[2] += dz;
    
    pMesh->bbMin[0] *= fSizeScale;
    pMesh->bbMax[0] *= fSizeScale;
    pMesh->bbMax[1] *= fSizeScale;
    pMesh->bbMin[2] *= fSizeScale;
    pMesh->bbMax[2] *= fSizeScale;
}


namespace Simpleton
{
    bool LoadPly( const char* pFileName, PlyMesh& rMesh, unsigned int Flags )
    {
        p_ply ply = ply_open(pFileName, NULL);
    
        if (!ply) 
            return false;

        if (!ply_read_header(ply)) 
            return false;

        // wipe the mesh first
        Context ctx;
        ctx.pMesh = &rMesh;
        ctx.pNextFace = 0;

        memset( ctx.pMesh, 0, sizeof(PlyMesh) );

        // set up callbacks for vertex data
        UINT nVertices = ply_set_read_cb( ply, "vertex", "x", &VertexCallback, &ctx, 0);
                         ply_set_read_cb( ply, "vertex", "y", &VertexCallback, &ctx, 1);
                         ply_set_read_cb( ply, "vertex", "z", &VertexCallback, &ctx, 2);

        UINT nNormals=0;
        UINT nUVs=0;
        UINT nFaceColors=0;
        UINT nVertexColors=0;

        if( !(Flags & PF_IGNORE_NORMALS) )
        {
            nNormals =  ply_set_read_cb( ply, "vertex", "nx", &VertexNormalCallback, &ctx, 0)&&
                             ply_set_read_cb( ply, "vertex", "ny", &VertexNormalCallback, &ctx, 1)&&
                             ply_set_read_cb( ply, "vertex", "nz", &VertexNormalCallback, &ctx, 2);
        }

        if( !(Flags & PF_IGNORE_COLORS) )
        {
            nFaceColors = ply_set_read_cb( ply, "face", "red",   FaceColorCallback, &ctx, 0 ) && 
                           ply_set_read_cb( ply, "face", "green", FaceColorCallback, &ctx, 1 ) && 
                           ply_set_read_cb( ply, "face", "blue",  FaceColorCallback, &ctx, 2 );

            nVertexColors = ply_set_read_cb( ply, "vertex", "red",   VertexColorCallback, &ctx, 0 ) && 
                            ply_set_read_cb( ply, "vertex", "green", VertexColorCallback, &ctx, 1 ) && 
                            ply_set_read_cb( ply, "vertex", "blue",  VertexColorCallback, &ctx, 2 );
        }

        if( !(Flags & PF_IGNORE_UVS) )
        {
            nUVs =  (ply_set_read_cb( ply, "vertex", "s", &VertexUVCallback, &ctx, 0) &&
                     ply_set_read_cb( ply, "vertex", "t", &VertexUVCallback, &ctx, 1)) ||
                    (ply_set_read_cb( ply, "vertex", "u", &VertexUVCallback, &ctx, 0) &&
                     ply_set_read_cb( ply, "vertex", "v", &VertexUVCallback, &ctx, 1)) ||
                    (ply_set_read_cb( ply, "vertex", "texture_u", &VertexUVCallback, &ctx, 0) &&
                     ply_set_read_cb( ply, "vertex", "texture_v", &VertexUVCallback, &ctx, 1));
        }
    
        // allocate arrays based on vertex counts and components which are present
        ctx.pMesh->nVertices = nVertices;
        ctx.pMesh->pPositions = new PlyMesh::Float3[nVertices];
        for( UINT i=0; i<3; i++ )
        {
            ctx.pMesh->bbMin[i] = FLT_MAX;
            ctx.pMesh->bbMax[i] = -FLT_MAX;
        }

        if( nNormals || (Flags&PF_REQUIRE_NORMALS))
            ctx.pMesh->pNormals = new PlyMesh::Float3[nVertices];

        if( nUVs )
            ctx.pMesh->pUVs = new PlyMesh::Float2[nVertices];
    
        if( nFaceColors )
        {
            ctx.pMesh->pFaceColors = new PlyMesh::Color[nFaceColors];        // there are per-face colors.  Allocate space for them
            memset( ctx.pMesh->pFaceColors,0xff,sizeof(PlyMesh::Color)*nFaceColors );
        }

        if( nVertexColors )
        {
            ctx.pMesh->pVertexColors = new PlyMesh::Color[nVertices];        // there are per-face colors.  Allocate space for them
            memset( ctx.pMesh->pVertexColors,0xff,sizeof(PlyMesh::Color)*nVertices );
        }

    

        // we'll support triangle lists, or a single large strip with cuts, but not both of them...
        // in the strip case, we need to defer allocation until the callback because we won't know the strip length
        UINT nFaces = ply_set_read_cb(ply, "face", "vertex_indices", FaceListCallback, &ctx, 0 );    
        UINT nStrips = ply_set_read_cb(ply, "tristrips", "vertex_indices", TriStripCallback, &ctx, 0);
        if( nFaces )
        {
            ctx.pMesh->nTriangles = nFaces;
            ctx.pMesh->pVertexIndices = new uint32[nFaces*3];
        }
        else if( nStrips != 1 )
        {
            // we can't parse multiple strips, because we won't know how many faces we need to create
            // and 0 strips probably mean we don't have a clue what this file type is
            ply_close(ply);
            return false;
        }


        bool ok = ply_read(ply) != 0;
        ply_close(ply);

        if( ok )
        {
            // if we had strips, figure out how many triangles were actually created
            if( ctx.pNextFace )
                ctx.pMesh->nTriangles = (ctx.pNextFace - ctx.pMesh->pVertexIndices) / 3;

            if( Flags & (PF_STANDARDIZE_POSITIONS) )
                PostProcessPositions( ctx.pMesh );
            
            // create vertex normals from face normals if not present
            if( (Flags & (PF_REQUIRE_NORMALS)) && !nNormals )
            {
                auto _ReadPosition = 
                    [&ctx]( float* pPosition, uint i ) 
                    { 
                        for( uint k=0;k<3;k++ )
                            pPosition[k] = ctx.pMesh->pPositions[i][k];
                    };
                auto _ReadNormal = 
                    [&ctx]( float* pNormal, uint i ) 
                    { 
                        for( uint k=0;k<3;k++ )
                            pNormal[k] = ctx.pMesh->pNormals[i][k];
                    };
                auto _WriteNormal = 
                    [&ctx]( const float* pNormal, uint i ) 
                    { 
                        for( uint k=0;k<3;k++ )
                            ctx.pMesh->pNormals[i][k] = pNormal[k];
                    };

                ComputeVertexNormals( ctx.pMesh->pVertexIndices, ctx.pMesh->nTriangles, ctx.pMesh->nVertices,
                                      _ReadPosition,
                                      _ReadNormal,
                                      _WriteNormal );
            }
        }
  
        return ok;
    }


    bool WritePly( const char* pWhere, PlyMesh& mesh )
    {
        p_ply ply = ply_create(pWhere,PLY_LITTLE_ENDIAN, NULL);
    
        if (!ply) 
            return false;


        ply_add_element( ply, "vertex", mesh.nVertices );
        ply_add_scalar_property( ply, "x", PLY_FLOAT32 );
        ply_add_scalar_property( ply, "y", PLY_FLOAT32 );
        ply_add_scalar_property( ply, "z", PLY_FLOAT32 );
        if( mesh.pNormals )
        {
            ply_add_scalar_property( ply, "nx", PLY_FLOAT32 );
            ply_add_scalar_property( ply, "ny", PLY_FLOAT32 );
            ply_add_scalar_property( ply, "nz", PLY_FLOAT32 );
        }
        if( mesh.pUVs )
        {
            ply_add_scalar_property( ply, "u", PLY_FLOAT32 );
            ply_add_scalar_property( ply, "v", PLY_FLOAT32 );
        }
        if( mesh.pVertexColors )
        {
            ply_add_scalar_property( ply, "red",   PLY_UCHAR );
            ply_add_scalar_property( ply, "green", PLY_UCHAR );
            ply_add_scalar_property( ply, "blue",  PLY_UCHAR );
            ply_add_scalar_property( ply, "alpha", PLY_UCHAR );
        }
        
        ply_add_element( ply, "face", mesh.nTriangles );
 
        ply_add_list_property( ply, "vertex_indices", PLY_UCHAR, PLY_UIN32 );
 

        if( mesh.pFaceColors )
        {
            ply_add_scalar_property( ply, "red", PLY_UCHAR );
            ply_add_scalar_property( ply, "green", PLY_UCHAR );
            ply_add_scalar_property( ply, "blue", PLY_UCHAR );
            ply_add_scalar_property( ply, "alpha", PLY_UCHAR );
        }

        if (!ply_write_header(ply)) 
            return false;

        for( size_t v=0; v<mesh.nVertices; v++ )
        {
            ply_write( ply, mesh.pPositions[v][0] );
            ply_write( ply, mesh.pPositions[v][1] );
            ply_write( ply, mesh.pPositions[v][2] );
            if( mesh.pNormals )
            {
                ply_write( ply, mesh.pNormals[v][0] );
                ply_write( ply, mesh.pNormals[v][1] );
                ply_write( ply, mesh.pNormals[v][2] );
            }
            if( mesh.pUVs )
            {
                ply_write( ply, mesh.pUVs[v][0] );
                ply_write( ply, mesh.pUVs[v][1] );
            }
            if( mesh.pVertexColors )
            {
                ply_write( ply, mesh.pVertexColors[v].r );
                ply_write( ply, mesh.pVertexColors[v].g );
                ply_write( ply, mesh.pVertexColors[v].b );
                ply_write( ply, mesh.pVertexColors[v].a );
            }
        }
        
        for( size_t t=0; t<mesh.nTriangles; t++ )
        {
            ply_write(ply,3);
            ply_write(ply,mesh.pVertexIndices[3*t+0] );
            ply_write(ply,mesh.pVertexIndices[3*t+1] );
            ply_write(ply,mesh.pVertexIndices[3*t+2] );

            if( mesh.pFaceColors )
            {
                ply_write( ply, mesh.pFaceColors[t].r );
                ply_write( ply, mesh.pFaceColors[t].g );
                ply_write( ply, mesh.pFaceColors[t].b );
                ply_write( ply, mesh.pFaceColors[t].a );
            }
        }

        ply_close(ply);
        return true;
    }


    void FreePly( PlyMesh& rMesh )
    {
        if( rMesh.pNormals )
            delete[] rMesh.pNormals;
        if( rMesh.pUVs )
            delete[] rMesh.pUVs;
        if( rMesh.pPositions )
            delete[] rMesh.pPositions;
        if( rMesh.pFaceColors )
            delete[] rMesh.pFaceColors;
        if( rMesh.pVertexIndices )
            delete[] rMesh.pVertexIndices;

        rMesh.pNormals=0;
        rMesh.pUVs=0;
        rMesh.pPositions=0;
        rMesh.pFaceColors=0;
        rMesh.pVertexIndices=0;
    }

}