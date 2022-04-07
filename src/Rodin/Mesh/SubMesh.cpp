#include "SubMesh.h"

namespace Rodin
{
   SubMesh::SubMesh(const SubMesh& other)
      :  Mesh(other),
         m_parent(other.m_parent),
         m_s2pv(other.m_s2pv)
   {}

   const std::map<int, int>& SubMesh::getVertexMap() const
   {
      assert(m_s2pv);
      return *m_s2pv;
   }

   SubMesh& SubMesh::setVertexMap(std::map<int, int> s2pv)
   {
      m_s2pv = s2pv;
      return *this;
   }

   SubMesh& SubMesh::setParent(const MeshBase& parent)
   {
      m_parent = std::cref(parent);
      return *this;
   }

   const MeshBase& SubMesh::getParent() const
   {
      assert(m_parent);
      return m_parent->get();
   }
}
