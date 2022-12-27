/*! \file trpp_tests.cpp
    \brief tests for the Triangle++ code
 */

#include "tpp_interface.hpp"

#include <catch2/catch_test_macros.hpp>

#include <vector>
#include <iostream>
#include <cassert>

// debug support
#define DEBUG_OUTPUT_STDOUT false 

using namespace tpp;

namespace {

#if DEBUG_OUTPUT_STDOUT
    tpp::DebugOutputLevel dbgOutput = tpp::Debug; // OR - tpp::Info
#else
    tpp::DebugOutputLevel dbgOutput = tpp::None;
#endif

    // impl. helpers

    void getTriangulationPoint(int keypointIdx, const Delaunay::Point& steinerPt, 
                               double& x, double& y, const std::vector<Delaunay::Point>& triPoints)
    {
        if (dbgOutput != tpp::None)
           std::cout << " --- keypointIdx= " << keypointIdx << "\n";

        if (keypointIdx == -1)
        {
            x = steinerPt[0]; // added Steiner point!
            y = steinerPt[1];
        }
        else
        {
            // point from original data
            x = triPoints[keypointIdx][0];
            y = triPoints[keypointIdx][1];
        }
    }
        
    void debugPrintTriangles(/*const*/ Delaunay& trGenerator, const std::vector<Delaunay::Point>& triPoints)
    {
        REQUIRE(trGenerator.hasTriangulation());

        if (dbgOutput == tpp::None)
        {
            return;
        }

        // iterate over triangles
        for (Delaunay::fIterator fit = trGenerator.fbegin(); fit != trGenerator.fend(); ++fit)
        {
            Delaunay::Point sp1;
            Delaunay::Point sp2;
            Delaunay::Point sp3;

            int keypointIdx1 = trGenerator.Org(fit, &sp1);
            int keypointIdx2 = trGenerator.Dest(fit, &sp2);
            int keypointIdx3 = trGenerator.Apex(fit, &sp3);

            double x1 = -1;
            double y1 = -1;
            double x2 = -1;
            double y2 = -1;
            double x3 = -1;
            double y3 = -1;

            getTriangulationPoint(keypointIdx1, sp1, x1, y1, triPoints);
            getTriangulationPoint(keypointIdx2, sp2, x2, y2, triPoints);
            getTriangulationPoint(keypointIdx3, sp3, x3, y3, triPoints);

            std::cout << " -- Triangle points: "
                << "{" << x1 << ", " << y1 << "}, "
                << "{" << x2 << ", " << y2 << "}, "
                << "{" << x3 << ", " << y3 << "}\n";
        }
    }

    void debugPrintVoronoiPoints(/*const*/ Delaunay& trGenerator)
    {
        REQUIRE(trGenerator.hasTriangulation());

        if (dbgOutput == tpp::None)
        {
            return;
        }

       // iterate over Voronoi points
       for (Delaunay::vvIterator vit = trGenerator.vvbegin(); vit != trGenerator.vvend(); ++vit)
       {
          Delaunay::Point vp = *vit;
          double x1 = vp[0];
          double y1 = vp[1];

          std::cout << " -- Voronoi point: " << "{" << x1 << "," << y1 << "}\n";
       }

       std::cout << " -- Voronoi points count: " << trGenerator.nvpoints() << "\n";
    }

    void checkTriangleCount(/*const*/ Delaunay& trGenerator, const std::vector<Delaunay::Point>& delaunayInput, 
                            int expected, const char* descr = nullptr)
    {
       debugPrintTriangles(trGenerator, delaunayInput);

       int triangleCt = trGenerator.ntriangles();

       if (dbgOutput != tpp::None)
          std::cout << " -- " << (descr ? descr : "") << " triangle count: " << triangleCt << "\n";

       REQUIRE(triangleCt == expected);
    }

    bool checkConstraints(const Delaunay& trGenerator)
    {
        bool relaxedTest = true;

        if (!trGenerator.checkConstraintsOpt(relaxedTest))
        {
            if (dbgOutput != tpp::None)
               std::cout << " -- constraints out of bounds!!!\n";

            return false;
        }

        return true;
    };

}


// tests

TEST_CASE("unconstrained triangulation", "[trpp]")
{
    // prepare input
    std::vector<Delaunay::Point> delaunayInput;
    
    delaunayInput.push_back(Delaunay::Point(0,0));
    delaunayInput.push_back(Delaunay::Point(1,1));
    delaunayInput.push_back(Delaunay::Point(0,2));
    delaunayInput.push_back(Delaunay::Point(3,3));
    delaunayInput.push_back(Delaunay::Point(1.5, 2.125));

    // 1. standard triangulation

    Delaunay trGenerator(delaunayInput);
    int triangleCt = 0;
    int expected = 0;

    SECTION("TEST 1: standard triangulation") 
    {
       trGenerator.Triangulate(dbgOutput);

       expected = 4;
       checkTriangleCount(trGenerator, delaunayInput, expected, "Standard");
    }

    // 2. triangulate with quality constraints

    bool withQuality = true;

    SECTION("TEST 2.1: default constraints (min angle = 20�)") 
    {
       trGenerator.Triangulate(withQuality, dbgOutput);    

       expected = 7;
       checkTriangleCount(trGenerator, delaunayInput, expected, "Quality");
    }

    SECTION("TEST 2.2: custom constraints (angle = 27.5�)") 
    {
       trGenerator.setMinAngle(27.5f);
       REQUIRE(checkConstraints(trGenerator) == true);

       trGenerator.Triangulate(withQuality, dbgOutput);

       expected = 11;
       checkTriangleCount(trGenerator, delaunayInput, expected);
    }

    SECTION("TEST 2.3: custom constraints (angle = 30.5�, area = 5.5)") 
    {
       trGenerator.setMinAngle(30.5f);
       trGenerator.setMaxArea(5.5f);
       REQUIRE(checkConstraints(trGenerator) == true);

       trGenerator.Triangulate(withQuality, dbgOutput);

       expected = 17;
       checkTriangleCount(trGenerator, delaunayInput, expected);
    }

    SECTION("TEST 2.4: custom constraints (angle = 44�)") 
    {
       // 44 deg results in an endless loop 
       //  --> triangles too tiny for the floating point precision! 
       trGenerator.setMinAngle(44.0f);
       trGenerator.setMaxArea(-1);

       REQUIRE(checkConstraints(trGenerator) == false);               
    }

    // 3. Voronoi diagrams    

    SECTION("TEST 3: Voronoi tesselation") 
    {
       trGenerator.Tesselate();       
       debugPrintVoronoiPoints(trGenerator);

       int voronoiPoints = trGenerator.nvpoints();
       expected = 4;

       REQUIRE(voronoiPoints == expected);              
    }
}


TEST_CASE("segment-constrainded triangluation (CDT)", "[trpp]")
{
    // prepare input 
    //  - see "example constr segments.jpg" for visualisation!
    std::vector<Delaunay::Point> constrDelaunayInput;
    
    constrDelaunayInput.push_back(Delaunay::Point(0, 0));
    constrDelaunayInput.push_back(Delaunay::Point(0, 1));
    constrDelaunayInput.push_back(Delaunay::Point(0, 3));
    constrDelaunayInput.push_back(Delaunay::Point(2, 0));
    constrDelaunayInput.push_back(Delaunay::Point(4, 1.25));
    constrDelaunayInput.push_back(Delaunay::Point(4, 3));
    constrDelaunayInput.push_back(Delaunay::Point(6, 0));
    constrDelaunayInput.push_back(Delaunay::Point(8, 1.25));
    constrDelaunayInput.push_back(Delaunay::Point(9, 0));
    constrDelaunayInput.push_back(Delaunay::Point(9, 0.75));
    constrDelaunayInput.push_back(Delaunay::Point(9, 3));

    int expected = 0;
    int referenceCt = 0;
    int referenceQualityCt = 0;
    bool withQuality = true;

    Delaunay trConstrGenerator(constrDelaunayInput);

    SECTION("TEST 4.0: reference triangulation (without quality constr.)")
    {
        trConstrGenerator.Triangulate(!withQuality, dbgOutput);
        referenceCt = trConstrGenerator.ntriangles();

        expected = 11;
        checkTriangleCount(trConstrGenerator, constrDelaunayInput, expected, "Unconstrained (quality=false)");

        REQUIRE(referenceCt == expected);
    }

    //trConstrGenerator.setMinAngle(30.5f);
    //trConstrGenerator.setMaxArea(5.5f);
    //REQUIRE(checkConstraints(trConstrGenerator) == true);

    SECTION("TEST 4.0: reference triangulation with quality constr.")
    {
        trConstrGenerator.Triangulate(withQuality, dbgOutput);
        referenceQualityCt = trConstrGenerator.ntriangles();

        expected = 11; // checked with GUI!
        checkTriangleCount(trConstrGenerator, constrDelaunayInput, expected, "Unconstrained (quality=true)");
    }

    // prepare segments 
    //  - see "example constr segments.jpg" for visualisation!
    std::vector<Delaunay::Point> constrDelaunaySegments;

    constrDelaunaySegments.push_back(Delaunay::Point(0, 1));
    constrDelaunaySegments.push_back(Delaunay::Point(9, 0.75));

    // 4. segment-constrained triangulation

    trConstrGenerator.setSegmentConstraint(constrDelaunaySegments);
    trConstrGenerator.useConvexHullWithSegments(true); // don't remove concavities!

    SECTION("TEST 4.1: CDT triangulation (without quality constr.)")
    {
        trConstrGenerator.Triangulate(dbgOutput);

        expected = 11; // count not changed, see "example constr segments.jpg" for visualisation!
        checkTriangleCount(trConstrGenerator, constrDelaunayInput, expected, "Constrained (quality=false)");

        // OPEN TODO:::
        //  -- but different triangles!!!! 
        //  CHECK( .... )
    }

    SECTION("TEST 4.2: CDT triangulation with quality constr.")
    {
        trConstrGenerator.Triangulate(withQuality, dbgOutput);

        expected = 29; // checked with GUI!
        checkTriangleCount(trConstrGenerator, constrDelaunayInput, expected, "Constrained (quality=true)");
    }

    // 5. triangulation with holes

    SECTION("TEST 5.1: holes + segment-constrainded triangluation (CDT)") 
    { 
       std::vector<Delaunay::Point> constrDelaunayHoles;

       constrDelaunayHoles.push_back(Delaunay::Point(5, 1));
       constrDelaunayHoles.push_back(Delaunay::Point(5, 2));
       constrDelaunayHoles.push_back(Delaunay::Point(6, 2));
       constrDelaunayHoles.push_back(Delaunay::Point(6, 1));

       trConstrGenerator.setHolesConstraint(constrDelaunayHoles);
       trConstrGenerator.Triangulate(withQuality, dbgOutput);
       
       expected = 11; // checked with GUI!
       checkTriangleCount(trConstrGenerator, constrDelaunayInput, expected, "Constrained + holes (quality=true)");

       trConstrGenerator.Triangulate(!withQuality, dbgOutput);

       expected = 4; // checked with GUI!
       checkTriangleCount(trConstrGenerator, constrDelaunayInput, expected, "Constrained + holes (quality=true)");
    }

    SECTION("TEST 5.1: holes + unconstrainded triangluation (CDT)") 
    { 
       std::vector<Delaunay::Point> zeroSegments;
       std::vector<Delaunay::Point> unconstrDelaunayHoles;

       unconstrDelaunayHoles.push_back(Delaunay::Point(0.25, 0.25));

       trConstrGenerator.setSegmentConstraint(zeroSegments);
       trConstrGenerator.setHolesConstraint(unconstrDelaunayHoles);

       trConstrGenerator.Triangulate(withQuality, dbgOutput);
   
       expected = 0; // all triangles infected as non edges required to be in triangualtion!
       checkTriangleCount(trConstrGenerator, constrDelaunayInput, expected, "Unconstrained + holes");
    }
}


TEST_CASE("Planar Straight Line Graph (PSLG) triangulation", "[trpp]")
{
    // prepare points: 
    //   - letter A, as in Triangle's documentation but simplified (https://www.cs.cmu.edu/~quake/triangle.defs.html#dt)
    std::vector<Delaunay::Point> pslgDelaunayInput;

    pslgDelaunayInput.push_back(Delaunay::Point(0, 0));
    pslgDelaunayInput.push_back(Delaunay::Point(1, 0));
    pslgDelaunayInput.push_back(Delaunay::Point(3, 0));
    pslgDelaunayInput.push_back(Delaunay::Point(4, 0));
    pslgDelaunayInput.push_back(Delaunay::Point(1.5, 1));
    pslgDelaunayInput.push_back(Delaunay::Point(2.5, 1));
    pslgDelaunayInput.push_back(Delaunay::Point(1.6, 1.5));
    pslgDelaunayInput.push_back(Delaunay::Point(2.4, 1.5));

    // TEST:::
#if 1 // OLD::
    pslgDelaunayInput.push_back(Delaunay::Point(2, 2));    
    pslgDelaunayInput.push_back(Delaunay::Point(3, 3));
#else
    // OPEN TODO::: test with GUI!!!!
    pslgDelaunayInput.push_back(Delaunay::Point(2, 2));
    pslgDelaunayInput.push_back(Delaunay::Point(1.25, 3));
    pslgDelaunayInput.push_back(Delaunay::Point(2.75, 3)); // ??? (3, 3)?
#endif

    // prepare segments 
    //   - letter A, as in Triangle's documentation but simplified (https://www.cs.cmu.edu/~quake/triangle.defs.html#dt)
    std::vector<Delaunay::Point> pslgDelaunaySegments;

    // outer outline
    pslgDelaunaySegments.push_back(Delaunay::Point(1, 0));
    pslgDelaunaySegments.push_back(Delaunay::Point(0, 0));           
    pslgDelaunaySegments.push_back(Delaunay::Point(0, 0));
    pslgDelaunaySegments.push_back(Delaunay::Point(3, 3));
    pslgDelaunaySegments.push_back(Delaunay::Point(3, 3));
    pslgDelaunaySegments.push_back(Delaunay::Point(4, 0));
    pslgDelaunaySegments.push_back(Delaunay::Point(4, 0));
    pslgDelaunaySegments.push_back(Delaunay::Point(3, 0));
    pslgDelaunaySegments.push_back(Delaunay::Point(3, 0));
    pslgDelaunaySegments.push_back(Delaunay::Point(2.5, 1));
    pslgDelaunaySegments.push_back(Delaunay::Point(2.5, 1));
    pslgDelaunaySegments.push_back(Delaunay::Point(1.5, 1));
    pslgDelaunaySegments.push_back(Delaunay::Point(1.5, 1));
    pslgDelaunaySegments.push_back(Delaunay::Point(1, 0));

    // inner outline
    pslgDelaunaySegments.push_back(Delaunay::Point(1.6, 1.5));
    pslgDelaunaySegments.push_back(Delaunay::Point(2, 2));
    pslgDelaunaySegments.push_back(Delaunay::Point(2, 2));
    pslgDelaunaySegments.push_back(Delaunay::Point(2.4, 1.5));
    pslgDelaunaySegments.push_back(Delaunay::Point(2.4, 1.5));
    pslgDelaunaySegments.push_back(Delaunay::Point(1.6, 1.5));

    // 6. Planar Straight Line Graph (PSLG) triangulations

    int expected = 0;
    bool withQuality = true;

    Delaunay trPlsgGenerator(pslgDelaunayInput);

    SECTION("TEST 6.1: Planar Straight Line Graph (PSLG) points-only triangluation") 
    {
       trPlsgGenerator.Triangulate(/*withQuality = false*/ dbgOutput); // ???? OPEN::: can be done?

       expected = 12;
       checkTriangleCount(trPlsgGenerator, pslgDelaunayInput, expected, "Unconstrained");
    }

    SECTION("TEST 6.2: PSLG triangluation (quality=true)") 
    {
       bool segmentsOK = trPlsgGenerator.setSegmentConstraint(pslgDelaunaySegments);
       REQUIRE(segmentsOK);
       trPlsgGenerator.Triangulate(withQuality, dbgOutput);

       // OPEN TODO:::
       //expected = 50;
       expected = 34; // ???????

       checkTriangleCount(trPlsgGenerator, pslgDelaunayInput, expected, "Constrained (quality=true)");

       // OPEN TODO:: check --> concavities removed?
    }

    SECTION("TEST 6.3: PSLG triangluation (quality=false)") 
    {
       trPlsgGenerator.Triangulate(dbgOutput);

       // OPEN TODO:::
       //expected = 35;
       expected = 12; // ???????

       checkTriangleCount(trPlsgGenerator, pslgDelaunayInput, expected, "Constrained (quality=false)");
    }
}

// --- eof ---
