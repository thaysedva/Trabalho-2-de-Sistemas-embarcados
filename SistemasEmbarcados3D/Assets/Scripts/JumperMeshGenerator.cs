using System.Collections.Generic;
using UnityEngine;

namespace Assets.Scripts
{
    public class JumperMeshGenerator : MonoBehaviour
    {
        private const float JumperHeight = 0.0f;

        public GameObject Point1;
        public GameObject Point2;

        public bool AvoidStrangling;
        public float ColinearThreshold = 0.001f;
        public float elbowRadius = 0.5f;
        [Range(3, 32)] public int elbowSegments = 6;
        public bool FlatShading;
        public bool GenerateElbows = true;
        public bool generateEndCaps;
        public bool GenerateOnStart = true;
        public Material JumperMaterial;
        public float JumperRadius = 0.2f;

        [Range(3, 32)] public int jumperSegments = 8;
        public bool makeDoubleSided;

        // see README.md file for more information about the following parameters
        public List<Vector3> points;

        private void Start()
        {
            if (GenerateOnStart)
            {
                var distance = Vector3.Distance(Point1.transform.position, Point2.transform.position);
                var height = distance * 0.65f;

                var newHeight = height > 0.5 ? height : 0.5f;

                var newP0 = new Vector3(Point1.transform.position.x, Point1.transform.position.y + JumperHeight, Point1.transform.position.z);
                var newP1 = new Vector3(Point2.transform.position.x, Point2.transform.position.y + JumperHeight, Point2.transform.position.z);

                var p0Height = new Vector3(Point1.transform.position.x, Point1.transform.position.y + JumperHeight + newHeight,
                    Point1.transform.position.z);
                var p1Height = new Vector3(Point2.transform.position.x, Point2.transform.position.y + JumperHeight + newHeight,
                    Point2.transform.position.z);

                points = new List<Vector3> { newP0 };

                var numPoints = 8 * distance;

                for (var i = 1; i <= numPoints; i++)
                {
                    var t = i / numPoints;
                    var pixel = CalculateCubicBezierPoint(t, newP0,
                        p0Height,
                        p1Height,
                        newP1);
                    points.Add(pixel);
                }

                points.Add(newP1);
                RenderJumper();
            }
        }

        private Vector3 CalculateCubicBezierPoint(float t, Vector3 p0, Vector3 p0Height, Vector3 p1Height, Vector3 p1)
        {
            var u = 1 - t;
            var tt = t * t;
            var uu = u * u;
            var uuu = uu * u;
            var ttt = tt * t;

            var p = uuu * p0;
            p += 3 * uu * t * p0Height;
            p += 3 * u * tt * p1Height;
            p += ttt * p1;

            return p;
        }

        public void RenderJumper()
        {
            if (points != null && points.Count < 2) return;

            // remove any colinear points, as creating elbows between them
            // would result in a torus of infinite radius, which is generally
            // frowned upon. also, it helps in keeping the triangle count low. :)
            RemoveColinearPoints();

            // add mesh filter if not present
            var currentMeshFilter = GetComponent<MeshFilter>();
            var mf = currentMeshFilter != null ? currentMeshFilter : gameObject.AddComponent<MeshFilter>();
            var mesh = GenerateMesh();

            if (FlatShading)
                mesh = MakeFlatShading(mesh);
            if (makeDoubleSided)
                mesh = MakeDoubleSided(mesh);
            mf.mesh = mesh;

            // add mesh renderer if not present
            var currentMeshRenderer = GetComponent<MeshRenderer>();
            var mr = currentMeshRenderer != null ? currentMeshRenderer : gameObject.AddComponent<MeshRenderer>();
            mr.materials = new Material[1] {JumperMaterial};

            if (points.Count > 2)
            {
                var currentCollision = GetComponent<MeshCollider>();
                var mc = currentCollision != null ? currentCollision : gameObject.AddComponent<MeshCollider>();

                mc.sharedMesh = mf.mesh;
            }
        }

        private Mesh GenerateMesh()
        {
            var m = new Mesh {name = "JumperMesh"};
            var vertices = new List<Vector3>();
            var triangles = new List<int>();
            var normals = new List<Vector3>();

            // for each segment, generate a cylinder
            for (var i = 0; i < points.Count - 1; i++)
            {
                var initialPoint = points[i];
                var endPoint = points[i + 1];
                var direction = (points[i + 1] - points[i]).normalized;

                if (i > 0 && GenerateElbows) // leave space for the elbow that will connect to the previous
                    // segment, except on the very first segment
                    initialPoint = initialPoint + direction * elbowRadius;

                if (i < points.Count - 2 && GenerateElbows) // leave space for the elbow that will connect to the next
                    // segment, except on the last segment
                    endPoint = endPoint - direction * elbowRadius;

                // generate two circles with "jumperSegments" sides each and then
                // connect them to make the cylinder
                GenerateCircleAtPoint(vertices, normals, initialPoint, direction);
                GenerateCircleAtPoint(vertices, normals, endPoint, direction);
                MakeCylinderTriangles(triangles, i);
            }

            // for each segment generate the elbow that connects it to the next one
            if (GenerateElbows)
                for (var i = 0; i < points.Count - 2; i++)
                {
                    var point1 = points[i]; // starting point
                    var point2 = points[i + 1]; // the point around which the elbow will be built
                    var point3 = points[i + 2]; // next point
                    GenerateElbow(i, vertices, normals, triangles, point1, point2, point3);
                }

            if (generateEndCaps) GenerateEndCaps(vertices, triangles, normals);

            m.SetVertices(vertices);
            m.SetTriangles(triangles, 0);
            m.SetNormals(normals);
            return m;
        }

        private void RemoveColinearPoints()
        {
            var pointsToRemove = new List<int>();
            for (var i = 0; i < points.Count - 2; i++)
            {
                var point1 = points[i];
                var point2 = points[i + 1];
                var point3 = points[i + 2];

                var dir1 = point2 - point1;
                var dir2 = point3 - point2;

                // check if their directions are roughly the same by
                // comparing the distance between the direction vectors
                // with the threshold
                if (Vector3.Distance(dir1.normalized, dir2.normalized) < ColinearThreshold) pointsToRemove.Add(i + 1);
            }

            pointsToRemove.Reverse();
            foreach (var idx in pointsToRemove) points.RemoveAt(idx);
        }

        private void GenerateCircleAtPoint(List<Vector3> vertices, List<Vector3> normals, Vector3 center,
            Vector3 direction)
        {
            // 'direction' is the normal to the plane that contains the circle

            // define a couple of utility variables to build circles
            var twoPi = Mathf.PI * 2;
            var radiansPerSegment = twoPi / jumperSegments;

            // generate two axes that define the plane with normal 'direction'
            // we use a plane to determine which direction we are moving in order
            // to ensure we are always using a left-hand coordinate system
            // otherwise, the triangles will be built in the wrong order and
            // all normals will end up inverted!
            var p = new Plane(Vector3.forward, Vector3.zero);
            var xAxis = Vector3.up;
            var yAxis = Vector3.right;
            if (p.GetSide(direction)) yAxis = Vector3.left;

            // build left-hand coordinate system, with orthogonal and normalized axes
            Vector3.OrthoNormalize(ref direction, ref xAxis, ref yAxis);

            for (var i = 0; i < jumperSegments; i++)
            {
                var currentVertex =
                    center +
                    JumperRadius * Mathf.Cos(radiansPerSegment * i) * xAxis +
                    JumperRadius * Mathf.Sin(radiansPerSegment * i) * yAxis;
                vertices.Add(currentVertex);
                normals.Add((currentVertex - center).normalized);
            }
        }

        private void MakeCylinderTriangles(List<int> triangles, int segmentIdx)
        {
            // connect the two circles corresponding to segment segmentIdx of the jumper
            var offset = segmentIdx * jumperSegments * 2;
            for (var i = 0; i < jumperSegments; i++)
            {
                triangles.Add(offset + (i + 1) % jumperSegments);
                triangles.Add(offset + i + jumperSegments);
                triangles.Add(offset + i);

                triangles.Add(offset + (i + 1) % jumperSegments);
                triangles.Add(offset + (i + 1) % jumperSegments + jumperSegments);
                triangles.Add(offset + i + jumperSegments);
            }
        }

        private void MakeElbowTriangles(List<Vector3> vertices, List<int> triangles, int segmentIdx, int elbowIdx)
        {
            // connect the two circles corresponding to segment segmentIdx of an
            // elbow with index elbowIdx
            var offset = (points.Count - 1) * jumperSegments * 2; // all vertices of cylinders
            offset += elbowIdx * (elbowSegments + 1) * jumperSegments; // all vertices of previous elbows
            offset += segmentIdx * jumperSegments; // the current segment of the current elbow

            // algorithm to avoid elbows strangling under dramatic
            // direction changes... we basically map vertices to the
            // one closest in the previous segment
            var mapping = new Dictionary<int, int>();
            if (AvoidStrangling)
            {
                var thisRingVertices = new List<Vector3>();
                var lastRingVertices = new List<Vector3>();

                for (var i = 0; i < jumperSegments; i++) lastRingVertices.Add(vertices[offset + i - jumperSegments]);

                for (var i = 0; i < jumperSegments; i++)
                {
                    // find the closest one for each vertex of the previous segment
                    var minDistVertex = Vector3.zero;
                    var minDist = Mathf.Infinity;
                    for (var j = 0; j < jumperSegments; j++)
                    {
                        var currentVertex = vertices[offset + j];
                        var distance = Vector3.Distance(lastRingVertices[i], currentVertex);
                        if (distance < minDist)
                        {
                            minDist = distance;
                            minDistVertex = currentVertex;
                        }
                    }

                    thisRingVertices.Add(minDistVertex);
                    mapping.Add(i, vertices.IndexOf(minDistVertex));
                }
            }
            else
            {
                // keep current vertex order (do nothing)
                for (var i = 0; i < jumperSegments; i++) mapping.Add(i, offset + i);
            }

            // build triangles for the elbow segment
            for (var i = 0; i < jumperSegments; i++)
            {
                triangles.Add(mapping[i]);
                triangles.Add(offset + i - jumperSegments);
                triangles.Add(mapping[(i + 1) % jumperSegments]);

                triangles.Add(offset + i - jumperSegments);
                triangles.Add(offset + (i + 1) % jumperSegments - jumperSegments);
                triangles.Add(mapping[(i + 1) % jumperSegments]);
            }
        }

        private Mesh MakeFlatShading(Mesh mesh)
        {
            // in order to achieve flat shading all vertices need to be
            // duplicated, because in Unity normals are assigned to vertices
            // and not to triangles.
            var newVertices = new List<Vector3>();
            var newTriangles = new List<int>();
            var newNormals = new List<Vector3>();

            for (var i = 0; i < mesh.triangles.Length; i += 3)
            {
                // for each face we need to clone vertices and assign normals
                var vertIdx1 = mesh.triangles[i];
                var vertIdx2 = mesh.triangles[i + 1];
                var vertIdx3 = mesh.triangles[i + 2];

                newVertices.Add(mesh.vertices[vertIdx1]);
                newVertices.Add(mesh.vertices[vertIdx2]);
                newVertices.Add(mesh.vertices[vertIdx3]);

                newTriangles.Add(newVertices.Count - 3);
                newTriangles.Add(newVertices.Count - 2);
                newTriangles.Add(newVertices.Count - 1);

                var normal = Vector3.Cross(
                    mesh.vertices[vertIdx2] - mesh.vertices[vertIdx1],
                    mesh.vertices[vertIdx3] - mesh.vertices[vertIdx1]
                ).normalized;
                newNormals.Add(normal);
                newNormals.Add(normal);
                newNormals.Add(normal);
            }

            mesh.SetVertices(newVertices);
            mesh.SetTriangles(newTriangles, 0);
            mesh.SetNormals(newNormals);

            return mesh;
        }

        private Mesh MakeDoubleSided(Mesh mesh)
        {
            // duplicate all triangles with inverted normals so the mesh
            // can be seen both from the outside and the inside
            var newTriangles = new List<int>(mesh.triangles);

            for (var i = 0; i < mesh.triangles.Length; i += 3)
            {
                var vertIdx1 = mesh.triangles[i];
                var vertIdx2 = mesh.triangles[i + 1];
                var vertIdx3 = mesh.triangles[i + 2];

                newTriangles.Add(vertIdx3);
                newTriangles.Add(vertIdx2);
                newTriangles.Add(vertIdx1);
            }

            mesh.SetTriangles(newTriangles, 0);

            return mesh;
        }

        private void GenerateElbow(int index, List<Vector3> vertices, List<Vector3> normals, List<int> triangles,
            Vector3 point1, Vector3 point2, Vector3 point3)
        {
            // generates the elbow around the area of point2, connecting the cylinders
            // corresponding to the segments point1-point2 and point2-point3
            var offset1 = (point2 - point1).normalized * elbowRadius;
            var offset2 = (point3 - point2).normalized * elbowRadius;
            var startPoint = point2 - offset1;
            var endPoint = point2 + offset2;

            // auxiliary vectors to calculate lines parallel to the edge of each
            // cylinder, so the point where they meet can be the center of the elbow
            var perpendicularToBoth = Vector3.Cross(offset1, offset2);
            var startDir = Vector3.Cross(perpendicularToBoth, offset1).normalized;
            var endDir = Vector3.Cross(perpendicularToBoth, offset2).normalized;

            // calculate torus arc center as the place where two lines projecting
            // from the edges of each cylinder intersect
            Vector3 torusCenter1;
            Vector3 torusCenter2;
            ClosestPointsOnTwoLines(out torusCenter1, out torusCenter2, startPoint, startDir, endPoint, endDir);
            var torusCenter = 0.5f * (torusCenter1 + torusCenter2);

            // calculate actual torus radius based on the calculated center of the 
            // torus and the point where the arc starts
            var actualTorusRadius = (torusCenter - startPoint).magnitude;

            var angle = Vector3.Angle(startPoint - torusCenter, endPoint - torusCenter);
            var radiansPerSegment = angle * Mathf.Deg2Rad / elbowSegments;
            var lastPoint = point2 - startPoint;

            for (var i = 0; i <= elbowSegments; i++)
            {
                // create a coordinate system to build the circular arc
                // for the torus segments center positions
                var xAxis = (startPoint - torusCenter).normalized;
                var yAxis = (endPoint - torusCenter).normalized;
                Vector3.OrthoNormalize(ref xAxis, ref yAxis);

                var circleCenter = torusCenter +
                                   actualTorusRadius * Mathf.Cos(radiansPerSegment * i) * xAxis +
                                   actualTorusRadius * Mathf.Sin(radiansPerSegment * i) * yAxis;

                var direction = circleCenter - lastPoint;
                lastPoint = circleCenter;

                if (i == elbowSegments) // last segment should always have the same orientation
                    // as the next segment of the jumper
                    direction = endPoint - point2;
                else if (i == 0) // first segment should always have the same orientation
                    // as the how the previous segmented ended
                    direction = point2 - startPoint;

                GenerateCircleAtPoint(vertices, normals, circleCenter, direction);

                if (i > 0) MakeElbowTriangles(vertices, triangles, i, index);
            }
        }

        private void GenerateEndCaps(List<Vector3> vertices, List<int> triangles, List<Vector3> normals)
        {
            // create the circular cap on each end of the jumper
            var firstCircleOffset = 0;
            var secondCircleOffset = (points.Count - 1) * jumperSegments * 2 - jumperSegments;

            vertices.Add(points[0]); // center of first segment cap
            var firstCircleCenter = vertices.Count - 1;
            normals.Add(points[0] - points[1]);

            vertices.Add(points[points.Count - 1]); // center of end segment cap
            var secondCircleCenter = vertices.Count - 1;
            normals.Add(points[points.Count - 1] - points[points.Count - 2]);

            for (var i = 0; i < jumperSegments; i++)
            {
                triangles.Add(firstCircleCenter);
                triangles.Add(firstCircleOffset + (i + 1) % jumperSegments);
                triangles.Add(firstCircleOffset + i);

                triangles.Add(secondCircleOffset + i);
                triangles.Add(secondCircleOffset + (i + 1) % jumperSegments);
                triangles.Add(secondCircleCenter);
            }
        }

        private bool ClosestPointsOnTwoLines(out Vector3 closestPointLine1, out Vector3 closestPointLine2,
            Vector3 linePoint1, Vector3 lineVec1, Vector3 linePoint2, Vector3 lineVec2)
        {
            closestPointLine1 = Vector3.zero;
            closestPointLine2 = Vector3.zero;

            var a = Vector3.Dot(lineVec1, lineVec1);
            var b = Vector3.Dot(lineVec1, lineVec2);
            var e = Vector3.Dot(lineVec2, lineVec2);

            var d = a * e - b * b;

            //lines are not parallel
            if (d != 0.0f)
            {
                var r = linePoint1 - linePoint2;
                var c = Vector3.Dot(lineVec1, r);
                var f = Vector3.Dot(lineVec2, r);

                var s = (b * f - c * e) / d;
                var t = (a * f - c * b) / d;

                closestPointLine1 = linePoint1 + lineVec1 * s;
                closestPointLine2 = linePoint2 + lineVec2 * t;

                return true;
            }

            return false;
        }
    }
}