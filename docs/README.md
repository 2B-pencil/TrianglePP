## Generating triangulations:

For usage patterns see the examples in the *trpp_example.cpp* source file. The interface of the *Tpp*-wrapper is defined in the *tpp_inteface.hpp* header file. 
Basic usage example is shown in the code snippet below:

    #include <tpp_interface.hpp>

    using namespace tpp;

    // prepare input
    std::vector<Delaunay::Point> delaunayInput = { ... };       

    // use standard triangulation
    Delaunay trGenerator(delaunayInput);
    trGenerator.Triangulate();

That's all! Now let have a look at the generated triangulation.

### Iterating over results

    // iterate over triangles
    for (FaceIterator fit = trGenerator.fbegin(); fit != trGenerator.fend(); ++fit)
    {
        int vertexIdx1 = fit.Org();  // queries the input data index!
        int vertexIdx2 = fit.Dest();
        int vertexIdx3 = fit.Apex();

        // access point's cooridinates: 
        double x1 = delaunayInput[vertexIdx1][0];
        double y1 = delaunayInput[vertexIdx1][1];
    }

Note that the *vertexIndex* is relative to the input vertex vector! In case of Steiner points (i.e. points which were added by the algorithm) *vertexIndex* will be equal -1. 
In that case you can access Steiner points coordinates using an optional parameter, ase shown below:

        int vertexIdx1 = fit.Org(); 
        if (vertexIdx1 < 0)
        {
            Delaunay::Point sp;
            f.Org(&sp);
        }

        // etc..

### foreach() style loop

You can also use the *foreach()* style loop as shown below:

    for (const auto& f : trGenerator.faces())
    {
        int vertexIdx1 = f.Org();  // queries the input data index!
        int vertexIdx2 = f.Dest();
        int vertexIdx3 = f.Apex();

        // etc...
    }

### Iterating using mesh indexes

As we already mentioned, the vertex indices returned by face iterator are relative to the input vertex vector!
Thus in case of Steiner points (i.e. points which were added by the algorithm) the returned *vertexIndex* will be -1, which requres an additional if-clause and breaks code flow.

To avoid that we can generate a continuous indexing for all the points of the triangulation, as shown below:

    Delaunay generator(inputPoints);
    generator.enableMeshIndexGeneration(); // must be enabled!

    generator.Triangulate(true);

    Delaunay::Point p0, p1, p2;
    int meshIdx0 = -1, meshIdx1 = -1, meshIdx2 = -1;

    for (auto fit = gen.fbegin(); fit != gen.fend(); ++fit)
    {
        fit.Org(p0, meshIdx0);  // queries the mesh index!
        fit.Dest(p1, meshIdx1);
        fit.Apex(p2, meshIdx2);
         
        ...
    }

In this case, the veratex coordinates will be always copied to the *point* parameter of the corresponding iterator's method.
t.b.c. ...

### Other iterators

t.b.c. ...

### Mesh walking

t.b.c. ...

### Quality constraints

t.b.c. ...

### Segment constraints

t.b.c. ...

## Generating Voronoi diagrams

To iterate over results of Voronoi tesselation two iterator classes are provided:

 - *VoronoiVertexIterator* which enumerates the Voronoi points, and 
 - *VoronoiEdgeIterator* which shows how these points are connected.

### Iterating over results

 **Example:**

    // get points:
    for (auto yiter = trGenerator.vvbegin(); yiter != trGenerator.vvend(); ++yiter)
    {
        // access data
        auto point = *yiter;
        double x = point[0];
        double y = point[1];

        // e.g.: draw point x,y
    }

    // ... and edges!
    for (auto eiter = trGenerator.vebegin(); eiter != trGenerator.veend(); ++eiter)
    {
        bool finiteEdge = false;
        Delaunay::Point pt1 = eiter.Org();
        Delaunay::Point pt2 = eiter.Dest(finiteEdge);

        // access data
        double xstart = pt1[0];
        double ystart = pt1[1];

        if (finiteEdge)
        {
            double xend = pt2[0];
            double yend = pt2[1];

            // e.g.: draw line pt1, pt2...
        }
        else
        {
            // an inifinite ray, thus no endpoint coordinates!
            auto rayNormalXValue = p2[0];
            auto rayNormalYValue = p2[1];
            assert(!(rayNormalXValue == 0.0 && rayNormalYValue == 0.0));

            // e.g.: draw an inifinite ray from pt1...
        }
    }


t.b.c. ...


## Traces and Logs

If compiled with *TRIANGLE_DBG_TO_FILE* define, debug traces will be written to the *./triangle.out.txt* file.

t.b.c. ...


## File I/O

You can write and read ASCII files containing point and segement definitions using Triangle++ methods. Some examples are stored in the *tppDataFiles* directory. 
Documentation for the used formats:

t.b.c. ...


### .node and .poly files

t.b.c. ...


### Input files sanitization

t.b.c. ...


### Example TrPP data files

examples in this directory are....


## Theory:

![Triangle logo](../T.gif) 


The original *Triangle* library documentation can be found at: http://www.cs.cmu.edu/~quake/triangle.html. The library was a **winner** of the 2003 James Hardy Wilkinson Prize in Numerical Software (sic!).

For backgroud info on the original implementation see "*Triangle: Engineering a 2D Quality Mesh Generator and Delaunay Triangulator*" by J.P. Shewchuk: http://www.cs.cmu.edu/~quake-papers/triangle.ps (or the local copy listed below!).

Algorithm used for DCT construction: "*Fast segment insertion and incremental construction of constrained Delaunay triangulations*", Shewchuk, J.R., Brown, B.C., Computational Geometry, Volume 48, Issue 8, September 2015, Pages 554-574 - https://doi.org/10.1016/j.comgeo.2015.04.006


### Local TriLib documantation files:
 
1. **triangle.pdf** - J.R. Shevchuk, *"Triangle: Engineering a 2D Quality Mesh Generator and Delaunay Triangulator"*, copy of: 'http://www.cs.cmu.edu/~quake-papers/triangle.ps'

2. **TriLib README.txt** - Docs extracted form Triangle's sources
