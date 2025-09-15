 #include "discreteGeometry.hpp"

using namespace glm;
 mat3 TriangulatedManifold::faceVertices(int i) { return mat3(triangles[polygroupIndexOrder.at(id)][i].getVertex(0).getPosition(),
																   triangles[polygroupIndexOrder.at(id)][i].getVertex(1).getPosition(), triangles[polygroupIndexOrder.at(id)][i].getVertex(2).getPosition()); }

 mat2x3 TriangulatedManifold::orthoFaceTangents(int i) const {
	 mat3 frame = tangentNormalFrameOfFace(i);
	 return mat2x3(frame[0], frame[1]);
 }

 Discrete1Form DiscreteRealFunctionManifold::exteriorDerivative() const {
	 vector<float> values = {};
	 values.reserve(domain->numEdges());
	 for (int i = 0; i < domain->numEdges(); i++)
		 values.emplace_back((values[domain->edgeVerticesIndices(i).x] - values[domain->edgeVerticesIndices(i).y])/edgeLength(i));
	 return Discrete1Form(values, domain);
 }

 FloatVector Discrete1Form::toBumpOrthoBasis() const { throw std::logic_error("Not implemented"); }
 Discrete1Form Discrete1Form::bumpOrtho(int i, int j) { throw std::logic_error("Not implemented"); }
 Discrete1Form Discrete1Form::bumpStd(int i, int j) { throw std::logic_error("Not implemented"); }
 float Discrete1Form::integrate(const vector<int> &edgePath) const { throw std::logic_error("Not implemented"); }
 Discrete2Form Discrete1Form::exteriorDerivative() const { throw std::logic_error("Not implemented"); }
