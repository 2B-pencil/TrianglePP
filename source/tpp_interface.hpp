
/*! \file tpp_interface.hpp
    \brief The main Delaunay C++ class of the Triangle++ wrapper.

    Use this class to produce Delaunay triangulations.

    The following description pertains to the original version, the current version
    was ported to VisualStudio and CMake by mrkkrj.
 */

 /*! \mainpage Triangle++
  \section intro Introduction

 If you do not know, what a Delaunay triangulation is, you can read more about it
 <a href="http://www.compgeom.com/~piyush/teach/5930/slides/lecture8.ppt">here</a> and
 <a href="http://en.wikipedia.org/wiki/Delaunay_triangulation">here</a>.
 This C++ library module is just a wrapper class on the <a href="http://www.cs.berkeley.edu/~jrs/">Triangle</a>
 package of <a href="http://www.cs.berkeley.edu/~jrs/">Jonathan Shewchuk</a>.

 Many times I have had to use triangle in C++ code bases of mine and have been forced to use C.
 At last I thought I would put a wrapper on his cool C code and it seems that this is what I got.

 The design is not perfect and the code was written in a day, but it does compile and run on the
 machines I tried (cygwin/redhat). The C++ wrapper will certainly slow access down if you want to
 mess with the triangulation but the basic delaunay triangulation should be as fast as triangle.

 Look at the tpp_interface.hpp file for getting started on what this wrapper can do for you. Also
 have a look at main.cpp which shows an example of using this class. The class is thread-safe.
 
 <img src="http://upload.wikimedia.org/wikipedia/en/9/92/Delaunay_triangulation.png" alt="Delaunay Triangulation Example">
 
 \section authors Authors
  <ul>
    <li><a href="http://compgeom.com/~piyush">Piyush Kumar</a></li>
    <li><a href="http://www.ib-krajewski.de">Marek Krajewski</a></li>
    <li>Hopefully more to come... (please feel free to extend this wrapper)</li>
  </ul>

 \section changelog Change Log

 11/03/06: Fixed the compilation system. <br>
 10/25/06: Wrapped in tpp namespace for usage with other libraries with similar names.
           Added some more documentation/small changes. Used doxygen 1.5.0 and dot. Tested compilation with
           icc 9.0/9.1, gcc-4.1/3.4.6. <br>
 10/21/06: Replaced vertexsort with C++ sort.<br>

 08/24/11: mrkkrj - Ported to VisualStudio, added comp. operators, reformatted and added some comments<br>
 10/15/11: mrkkrj - added support for the "quality triangulation" option, added some debug support<br>
 11/07/11: mrkkrj - bugfix in Triangle's divandconqdelauney() <br>
 17/09/18: mrkkrj � ported to 64-bit (preliminary, not thorougly tested!) <br>
 22/01/20: mrkkrj � added support for custom constraints (angle and area) <br>
 17/04/20: mrkkrj � added support Voronoi tesselation <br>
 05/08/22: mrkkrj � added more tests for constrained PSLG triangulations, 
                    included (reworked) Yejneshwar's fix for removal of concavities <br>
 17/12/22: mrkkrj � ported to Linux, reworked Yejneshwar's fix again<br>
 30/12/22: mrkkrj � added first file read-write support<br>

 \todo
 <ol>
   <li> Implement vertexmedian() in C++. </li>
   <li> Implement the flip operator as a member function of Delaunay. </li>
 </ol>
 */

//-----------------------------------------------------------

#ifndef TRPP_INTERFACE
#define TRPP_INTERFACE

#include "dpoint.hpp"

#include <vector>
#include <string>

struct triangulateio;


namespace tpp {

   // (mrkkrj)
   enum DebugOutputLevel 
   {
      None, 
      Info,    // most useful; it gives information on algorithmic progress and much more detailed statistics
      Vertex,  // gives vertex-by-vertex details, and prints so much that Triangle runs much more slowly
      Debug    // gives information only a debugger could love
   };


   //!  The main Delaunay Class that wraps around Triangle.
   /*!
     This is a C++ wrapper of the Triangle package by JP Shewchuk.

     This class currently uses the dpoint class written by me (the point class is a d-dimensional point
     class reviver::dpoint (but for this application it only uses the d=2 case).
     Additionally, the inner helper C++ class Triwrap groups the original Triangle's C functions.

     \note   (mrkkrj) For for backgroud info on the Triangle's implementation see "Triangle:
             Engineering a 2D Quality Mesh Generator and Delaunay Triangulator" by JR Shewchuk:
             www.cs.cmu.edu/~quake-papers/triangle.ps
   */
   class Delaunay 
   {
   public:
      //! Point Typedef
      /*! Warning: If you want to use your own point class, you might have to work hard...

           - mrkkrj: true!!! -> spare your time, use an adapter class!
      */
      typedef reviver::dpoint<double, 2> Point;

      //! The main constructor.
      /*!
        Takes a vector of 2 dimensional points where each of the coordinates is
        expressed as double.
      */
      Delaunay(const std::vector<Point>& points = std::vector<Point>());

      //! The main destructor.
      /*!
        Does memory cleanup mostly.
      */
      ~Delaunay();

      //! Delaunay triangulate the input points
      /*!
        This function calls Triangle.h to Delaunay-triangulate points given as input to the 
        constructor of this class. A quality triangualtion can be also created here.

        If segment constraints were set, this method creates a constrained Delaunay triangulation where 
        each PSLG segment is present as a single edge in the triangulation. Note that some of the resulting 
        triangles might *not be Delaunay*!

        \param quality  enforce minimal angle (default: 20�) and minimal area (only if explicitely set)
      */
      void Triangulate(bool quality = false, DebugOutputLevel traceLvl = None);

      void Triangulate(DebugOutputLevel traceLvl) {
          Triangulate(false, traceLvl);
      }

      //! Conforming Delaunay triangulate the input points 
      /*!
        This function calls Triangle.h to Delaunay-triangulate points given as input to the
        constructor of this class and the constraining segments set with setSegmentConstraint(). 
        Here a conforming triangualtion will be created.

        A conforming Delaunay triangulation is a *true Delaunay* triangulation in which each 
        constraining segment may have been subdivided into several edges by the insertion of additional 
        vertices, called Steiner points.

        \param quality  enforce minimal angle (default: 20�) and minimal area (only if explicitely set)
      */
      void TriangulateConf(bool quality = false, DebugOutputLevel traceLvl = None);

      void TriangulateConf(DebugOutputLevel traceLvl) {
          TriangulateConf(false, traceLvl);
      }

      //! Voronoi-tesselate the input points (added  mrkkrj)
      /*!
        This function calls triangle to create a Voronoi diagram with points given as input
        to the constructor of this class.

        Note that a Voronoi diagram can be only created if the underlying triangulation is convex 
        and doesn't have holes!

        \param useConformingDelaunay  use conforming Delaunay triangulation as base for the Voronoi diagram
      */
      void Tesselate(bool useConformingDelaunay = false, DebugOutputLevel traceLvl = None);

      //! Set a quality constraint for the triangulation
      /*!
        \param angle  min. resulting angle, if angle <= 0, the constraint will be removed.
      */
      void setMinAngle(float angle) {
         m_minAngle = angle;
      }

      //! Set a quality constraint for the triangulation
      /*!
        \param area  max. triangle area, if area <= 0, the constraint will be removed.
      */
      void setMaxArea(float area) {
         m_maxArea = area;
      }

      //! Set the segments to constrain the triangulation
      /*!
        Takes a vector of 2 dimensional points where each consecutive pair of points describes
        a single segment.

        Both endpoints of every segment are vertices of the input vector, and a segment may
        intersect other segments and vertices only at its endpoints.

        \return true if the input is valid, false otherwise 
      */
      bool setSegmentConstraint(const std::vector<Point>& segments);

     //! Set the segments to constrain the triangulation
     /*!
       Same as above, but usign indexes of the input points

       \return true if the input is valid, false otherwise
     */
     bool setSegmentConstraint(const std::vector<int>& segmentPointIndexes);

     //! Use convex hull when segments are set to constrain the triangulation
     /*!
       Option to generate convex hull using all specified points, the constraining segments are guaranteed 
       to be included in the triangulation
     */
     void useConvexHullWithSegments(bool useConvexHull);

     //! Set the holes to constrain the triangulation
     /*!
        OPEN TODO::: comment not correct????
       Takes a vector of 2 dimensional points where each consecutive pair of points describes a single edge of a hole.

       \return true if the input is valid, false otherwise
     */
     bool setHolesConstraint(const std::vector<Point>& holes);

      //! Are the quality constrainst sane?
      /*!
        \possible  true if is highly probable for triangualtion to succeed
        \return  true if triangualtion is guaranteed to succeed
      */
      bool checkConstraints(bool& possible) const;

      //! Are the quality constrainst sane, take two
      /*!
        \relaxed  report highly probable as correct too, as error otherwise
        \return  true if triangualtion is guaranteed to succeed, or at least higly probable to
      */
      bool checkConstraintsOpt(bool relaxed) const;

      //! Get minAngle intervals
      /*!
        \guaranteed  up to this value triangualtion is guaranteed to succeed
        \possible  up to this value it is highly probable for triangualtion to succeed
      */
      static void getMinAngleBoundaries(float& guaranteed, float& possible);

      //! Set a user test function for the triangulation
      /*!
        OPEN TODO::: NYI!!!
      */
      void setUserConstraint(bool (*f)()) { /* NYI !!!!! */ }

      //! Triangulation results, numbers of:
      int nedges() const;
      int ntriangles() const;
      int nvertices() const;
      int hull_size() const;
      int nholes() const;

      //! Tesselation results, numbers of:
      int nvpoints() const;
      int nvedges() const;

      //! Triangulation completed?
      bool hasTriangulation() const;

      //! Get min-max point coordinate values
      void getMinMaxPoints(double& minX, double& minY, double& maxX, double& maxY) const;

      //! Output a geomview .off file containing the delaunay triangulation
      void writeoff(std::string& fname);

      //! Save the vertices/segments to file. (added mrkkrj)
      bool savePoints(const std::string& filePath);
      bool saveSegments(const std::string& filePath);

      //! Read the vertices/segments from file. (added mrkkrj)
      bool readPoints(const std::string& filePath, std::vector<Point>& points);
      bool readSegments(const std::string& filePath, std::vector<Point>& points, std::vector<int>& segmentEndpoints,
                        std::vector<Delaunay::Point>& holeMarkers);


      ///////////////////////////////
      //
      // Vertex Iterator
      //
      ///////////////////////////////

      //!  The vertex iterator for the Delaunay class
      class vIterator {
      private:
         vIterator(Delaunay* tiangulator);   //! To set container

         Delaunay* MyDelaunay;   //! Which container do I point
         void* vloop;            //! Triangles Internal data.

      public:
         vIterator operator++();
         vIterator() :vloop(nullptr) {}
         Point& operator*() const;
         ~vIterator();

         friend class Delaunay;
         friend bool operator==(vIterator const&, vIterator const&);
         friend bool operator!=(vIterator const&, vIterator const&);
      };

      vIterator vbegin() { return vIterator(this); }
      vIterator vend();

      //! Given an iterator, find its index in the input vector of points.
      int vertexId(vIterator const& vit) const;

      //! Given an index, return the actual double Point
      const Point& point_at_vertex_id(int i) { return m_pointList[i]; }

#if 0 // NYI!
      //! Return the Point additionally created in quality mesh generation ("q" option)
      Point added_point_at_vertex_id(int i);
#endif

      friend class vIterator;


      ///////////////////////////////
      //
      // Face Iterator
      //
      ///////////////////////////////

      //!  The face iterator for the Delaunay class
      class fIterator {
      private:

         struct tdata {
            double*** tri;
            int orient;
         };

         typedef struct tdata  poface;

         fIterator(Delaunay* tiangulator);  //! To set container

         Delaunay* MyDelaunay;   //! Which container do I point
         //void *floop;          //! Triangles Internal data.
         poface floop;

      public:
         void operator++();
         fIterator() { floop.tri = nullptr; };
         ~fIterator();

         friend class Delaunay;
         friend bool operator==(fIterator const&, fIterator const&);
         friend bool operator!=(fIterator const&, fIterator const&);
         friend bool operator<(fIterator const&, fIterator const&); // added mrkkrj
      };

      fIterator fbegin() { return fIterator(this); };
      fIterator fend();

#if 0 // NYI!
      int faceId(fIterator const&);
#endif


      //! Access the origin (Org) vertex of a face.
      /*!
        A triangle abc has origin (org) a,destination (dest) b, and apex (apex)
        c.  These vertices occur in counterclockwise order about the triangle.
        Remember to call Triangulate before using this function. Do not use it on a null iterator.

        \param fit  Face interator.
        \param point  if specified: the cordinates of the vertex
        \return Index of the vertex in m_pList,
                or -1 if quality option was used and a new vertex was created!
      */
      int Org(fIterator const& fit, Point* point = 0);


      //! Access the destination (Dest) vertex of a face.
      /*!
        A triangle abc has origin (org) a,destination (dest) b, and apex (apex)
        c.  These vertices occur in counterclockwise order about the triangle.
        Remember to call Triangulate before using this function. Do not use it on a null iterator.

        \param fit  Face interator.
        \param point  if specified: the cordinates of the vertex
        \return Index of the vertex in m_pList,
                or -1 if quality option was used and a new vertex was created!
      */
      int Dest(fIterator const& fit, Point* point = 0);


      //! Access the apex (Apex) vertex of a face.
      /*!
        A triangle abc has origin (org) a,destination (dest) b, and apex (apex)
        c.  These vertices occur in counterclockwise order about the triangle.
        Remember to call Triangulate before using this function. Do not use it on a null iterator.

        \param fit  Face interator.
        \param point  if specified: the cordinates of the vertex
        \return Index of the vertex in m_pList,
                or -1 if quality option was used and a new vertex was created!
      */
      int Apex(fIterator const& fit, Point* point = 0);


      //! Access the triangle adjoining edge i
      /*!
        A triangle abc has origin (org) a,destination (dest) b, and apex (apex)
            c.  These vertices occur in counterclockwise order about the triangle.
        <ul>
        <li>sym(abc, 0) -> ba*</li>
        <li>sym(abc, 1) -> cb*</li>
        <li>sym(abc, 2) -> ac*</li>
        </ul>
        * is the farthest vertex on the adjoining triangle whose index
        is returned. A -1 is returned if the edge is part of the convex hull.
        Remember to call Triangulate before using this function.
        Do not use it on a null iterator.

        \param fit  Face Iterator
        \param i  edge number
        \return The vertex on the opposite face, or -1 (see Org() above)
      */
      int Sym(fIterator const& fit, char i);


      //! Access the triangle opposite to current edge of the face
      /*!
        A triangle abc has origin (org) a,destination (dest) b, and apex (apex)
            c.  These vertices occur in counterclockwise order about the triangle.
        The iterator
        to the triangle is returned. The iterator is empty if the edge
        is on the convex hull.
            Remember to call Triangulate before using this function.
        Do not use it on a null iterator.

        \param fit  Face iterator
        \return The iterator of the opposite face
      */
      fIterator Sym(fIterator const& fit);


      //! Is the iterator empty?
      /*!
        \param fit  Face interator.
        \return true if the iterator is empty
      */
      inline bool empty(fIterator const& fit)
      {
         return fit.floop.tri == nullptr;
      };


      //! Is the iterator pointing to the dummy triangle?
      /*!
        \param fit  Face interator.
        \return true if the iterator is of the dummy triangle.
      */
      bool isdummy(fIterator const& fit);


      //! Find the next edge (counterclockwise) of a triangle.   
      /*!
        Lnext(abc) -> bca.
        Remember to call Triangulate before using this function.
        Do not use it on a null iterator.

        \param fit  face iterator
        \return The face iterator corresponding to the next counterclockwise edge of a triangle
      */
      fIterator Lnext(fIterator const& fit);


      //! Find the previous edge (clockwise) of a triangle.   
      /*!
        Lprev(abc) -> cab.
        Remember to call Triangulate before using this function.
        Do not use it on a null iterator.

        \param fit  face iterator
        \return The face iterator corresponding to the previous clockwise edge of a triangle
      */
      fIterator Lprev(fIterator const& fit);


      //! Find the next edge (counterclockwise) of a triangle with the same origin
      /*!
        Onext(abc) -> ac*.
        Remember to call Triangulate before using this function.
        Do not use it on a null iterator.

        \param fit  face iterator
        \return The face iterator corresponding to the next edge counterclockwise with the same origin.
      */
      fIterator Onext(fIterator const& fit);


      //! Find the next edge clockwise with the same origin.
      /*!
        Onext(abc) -> a*b.
        Remember to call Triangulate before using this function.
        Do not use it on a null iterator.

        \param fit  face iterator
        \return The face iterator corresponding to the next edge clockwise with the same origin.
      */
      fIterator Oprev(fIterator const& fit);


      // TODO List: (for face iterators)
      /*  dnext:  Find the next edge counterclockwise with the same destination.   */
      /*  dnext(abc) -> *ba                                                        */
      /*                                                                           */
      /*  dprev:  Find the next edge clockwise with the same destination.          */
      /*  dprev(abc) -> cb*                                                        */
      /*                                                                           */
      /*  rnext:  Find the next edge (counterclockwise) of the adjacent triangle.  */
      /*  rnext(abc) -> *a*                                                        */
      /*                                                                           */
      /*  rprev:  Find the previous edge (clockwise) of the adjacent triangle.     */
      /*  rprev(abc) -> b**                                                        */


      //! Calculate incident triangles around a vertex.
      /*!
        Note that behaviour is undefined if vertexid is greater than
        number of vertices - 1. Remember to call Triangulate before using this function.
        All triangles returned have Org(triangle) = vertexid.
        All triangles returned are in counterclockwise order.

        \param vertexid The vertex for which you want incident triangles.
        \param ivv Returns triangles around a vertex in counterclockwise order.
      */
      void trianglesAroundVertex(int vertexid, std::vector<int>& ivv);


      //! Calculate the area of a face. 
      /*!
        \param fit  Face interator.
        \return area of the face associated with the iterator.
      */
      double area(fIterator const& fit);


      //! Point locate a vertex v
      /*!
        \param vertexid  vertex id
        \return a face iterator whose origin is v.
      */
      fIterator locate(int vertexid); // OPEN:: doesn't seem to be working!


      ///////////////////////////////
      //
      // Voronoi Points Iterator 
      //  (added mrkkrj)
      //
      ///////////////////////////////

      //!  The Voronoi points iterator for the Delaunay class
      class vvIterator {
      public:
         vvIterator();
         vvIterator operator++();
         Point& operator*() const;
         void advance(int steps);

      private:
         vvIterator(Delaunay* tiangulator);   //! To set container

         Delaunay* m_delaunay;    //! Which container do I point to
         void* vvloop;            //! Triangle's Internal data.
         int vvindex;
         int vvcount;

         friend class Delaunay;
         friend bool operator==(vvIterator const&, vvIterator const&);
         friend bool operator!=(vvIterator const&, vvIterator const&);
      };

      vvIterator vvbegin() { return vvIterator(this); }
      vvIterator vvend();


      ///////////////////////////////
      //
      // Voronoi Edges Iterator 
      //  (added mrkkrj)
      //
      ///////////////////////////////

      //!  The Voronoi edges iterator for the Delaunay class
      class veIterator {
      public:
         veIterator();
         veIterator operator++();
         int startPointId() const;
         int endPointId(Point& normvec) const;

      private:
         veIterator(Delaunay* tiangulator);   //! To set container

         Delaunay* m_delaunay;   //! Which container do I point to
         void* veloop;           //! Triangle's Internal data.
         int veindex;
         int vecount;

         friend class Delaunay;
         friend bool operator==(veIterator const&, veIterator const&);
         friend bool operator!=(veIterator const&, veIterator const&);
      };

      veIterator vebegin() { return veIterator(this); }
      veIterator veend();


      //! Access the origin (Org) vertex of an edge. (added mrkkrj)
      /*!
        \param eit  Voronoi Edge iterator.
        \return The start point of the Voronoi edge,

        Remember to call Tesselate before using this function. Do not use it on a null iterator.
      */
      const Point& Org(veIterator const& eit);


      //! Access the destination (Dest) vertex of an edge. (added mrkkrj)
      /*!
        \param eit  Voronoi Edge iterator.
        \param finiteEdge true for finite edges, false for inifinte rays.
        \return The end point of the Voronoi edge, for infinite rays the normal vector of the ray

        Remember to call Tesselate before using this function. Do not use it on a null iterator.
      */
      Point Dest(veIterator const& eit, bool& finiteEdge);


      //--------------------------------------
      // added mrkkrj - helper for Points 
      //    OPEN:: compiler cannot instantiate less<> with operator<() for Point, why?!
      //--------------------------------------
      struct OrderPoints
      {
         bool operator() (const Point& lhs, const Point& rhs) const {
            // first sort on x, then on y coordinates
            if (lhs[0] < rhs[0]) {
               return true;
            }
            if (lhs[0] == rhs[0] && lhs[1] < rhs[1]) {
               return true;
            }
            return false;
         }
      };

   private:
      void Triangulate(std::string& triswitches);

      // added mrkkrj - helper functions for face iterator access methods 
      //    HACK:: double* as not to export internal impl.
      void SetPoint(Point& point, double* vertexptr);
      int GetVertexIndex(fIterator const& fit, double* vertexptr);
      int GetFirstIndexNumber() const;

      // added mrkkrj 
      std::string formatFloatConstraint(float f) const;
      void setQualityOptions(std::string& options, bool quality);
      void setDebugLevelOption(std::string& options, DebugOutputLevel traceLvl);
      void freeTriangleDataStructs();
      void initTriangleDataForPoints();
      void initTriangleInputData(triangulateio* pin, const std::vector<Point>& points) const;
      void readPointsFromMesh(std::vector<Point>& points) const;
      void readSegmentsFromMesh(std::vector<int>& segments) const;

      friend class fIterator;

   private:
      std::vector<Point> m_pointList;   /*! Stores the input point list. */
      void* m_in;             /*! Used for intput to triangle  */
      void* m_triangleWrap;   /*! Triangle impl. is wrapped in this pointer. */
      void* m_pmesh;          /*! pointer to triangle mesh */
      void* m_pbehavior;
      bool m_triangulated;

      // added mrkkrj:
      void* m_vorout;          /*! pointer to Voronoi output */

      // added mrkkrj: quality constraints
      float m_minAngle;
      float m_maxArea;

     // added mrkkrj: segment constraints
     std::vector<int> m_segmentList;
     bool m_convexHullWithSegments;

     // added mrkkrj: holes 
     std::vector<Point> m_holesList;

   }; // Class Delaunay

} // namespace tpp ends.


#endif
