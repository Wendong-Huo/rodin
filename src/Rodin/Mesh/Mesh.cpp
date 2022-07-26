/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#include "Rodin/Alert.h"
#include "Rodin/IO/MeshLoader.h"
#include "Rodin/IO/MeshPrinter.h"
#include "Rodin/Variational/GridFunction.h"
#include "Rodin/Variational/FiniteElementSpace.h"

#include "Mesh.h"
#include "SubMesh.h"

#include "Element.h"

namespace Rodin
{
   // ---- MeshBase ----------------------------------------------------------
   int MeshBase::getSpaceDimension() const
   {
      return getHandle().SpaceDimension();
   }

   int MeshBase::getDimension() const
   {
      return getHandle().Dimension();
   }

   bool MeshBase::isSurface() const
   {
      return (getSpaceDimension() - 1 == getDimension());
   }

   void MeshBase::refine()
   {
      getHandle().UniformRefinement();
   }

   std::set<int> MeshBase::getAttributes() const
   {
      return std::set<int>(
            getHandle().attributes.begin(), getHandle().attributes.end());
   }

   std::set<int> MeshBase::getBoundaryAttributes() const
   {
      return std::set<int>(
            getHandle().bdr_attributes.begin(), getHandle().bdr_attributes.end());
   }

   void Mesh<Traits::Serial>::save(
         const boost::filesystem::path& filename, IO::FileFormat fmt, int precision) const
   {
      std::ofstream ofs(filename.c_str());
      if (!ofs)
      {
         Alert::Exception()
            << "Failed to open " << filename << " for writing."
            << Alert::Raise;
      }
      ofs.precision(precision);
      switch (fmt)
      {
         case IO::FileFormat::MFEM:
         {
            IO::MeshPrinter<IO::FileFormat::MFEM, Traits::Serial> printer(*this);
            printer.print(ofs);
            break;
         }
         case IO::FileFormat::GMSH:
         {
            IO::MeshPrinter<IO::FileFormat::GMSH, Traits::Serial> printer(*this);
            printer.print(ofs);
            break;
         }
         case IO::FileFormat::MEDIT:
         {
            IO::MeshPrinter<IO::FileFormat::MEDIT, Traits::Serial> printer(*this);
            printer.print(ofs);
            break;
         }
         default:
         {
            Alert::Exception()
               << "Saving to \"" << fmt << "\" format unssuported."
               << Alert::Raise;
         }
      }
   }

   MeshBase& MeshBase::displace(const Variational::GridFunctionBase& u)
   {
      assert(u.getFiniteElementSpace().getVectorDimension() == getDimension());
      getHandle().MoveNodes(u.getHandle());
      return *this;
   }

   double
   MeshBase::getMaximumDisplacement(const Variational::GridFunctionBase& u)
   {
      double res;
      getHandle().CheckDisplacements(u.getHandle(), res);
      return res;
   }

   double MeshBase::getVolume()
   {
      double totalVolume = 0;
      for (int i = 0; i < count<Element>(); i++)
         totalVolume += getHandle().GetElementVolume(i);
      return totalVolume;
   }

   double MeshBase::getVolume(int attr)
   {
      double totalVolume = 0;
      for (int i = 0; i < count<Element>(); i++)
         totalVolume += getHandle().GetElementVolume(i) * (getHandle().GetAttribute(i) == attr);
      return totalVolume;
   }

   double MeshBase::getBoundaryElementArea(int i)
   {
      mfem::ElementTransformation *et = getHandle().GetBdrElementTransformation(i);
      const mfem::IntegrationRule &ir = mfem::IntRules.Get(
            getHandle().GetBdrElementBaseGeometry(i), et->OrderJ());
      double area = 0.0;
      for (int j = 0; j < ir.GetNPoints(); j++)
      {
         const mfem::IntegrationPoint &ip = ir.IntPoint(j);
         et->SetIntPoint(&ip);
         area += ip.weight * et->Weight();
      }
      return area;
   }

   double MeshBase::getPerimeter()
   {
      double totalArea = 0;
      for (int i = 0; i < count<Element>(); i++)
         totalArea += getBoundaryElementArea(i);
      return totalArea;
   }

   double MeshBase::getPerimeter(int attr)
   {
      double totalVolume = 0;
      for (int i = 0; i < count<BoundaryElement>(); i++)
         totalVolume += getBoundaryElementArea(i) * (getHandle().GetBdrAttribute(i) == attr);
      return totalVolume;
   }

   std::set<int> MeshBase::where(std::function<bool(const Element&)> p) const
   {
      std::set<int> res;
      for (int i = 0; i < count<Element>(); i++)
         if (p(get<Element>(i)))
            res.insert(i);
      return res;
   }

   MeshBase& MeshBase::edit(std::function<void(ElementView)> f)
   {
      for (int i = 0; i < count<Element>(); i++)
         f(get<Element>(i));
      return *this;
   }

   MeshBase& MeshBase::edit(std::function<void(BoundaryElementView)> f)
   {
      for (int i = 0; i < count<BoundaryElement>(); i++)
         f(get<BoundaryElement>(i));
      return *this;
   }

   MeshBase& MeshBase::edit(std::function<void(ElementView)> f, const std::set<int>& elements)
   {
      for (auto el : elements)
      {
         assert(el >= 0);
         assert(el < count<Element>());

         f(get<Element>(el));
      }
      return *this;
   }

   MeshBase& MeshBase::update()
   {
      getHandle().SetAttributes();
      return *this;
   }

   std::deque<std::set<int>> MeshBase::ccl(
         std::function<bool(const Element&, const Element&)> p) const
   {
      std::set<int> visited;
      std::deque<int> searchQueue;
      std::deque<std::set<int>> res;

      // Perform the labelling
      for (int i = 0; i < count<Element>(); i++)
      {
         if (!visited.count(i))
         {
            res.push_back({});
            searchQueue.push_back(i);
            while (searchQueue.size() > 0)
            {
               int el = searchQueue.back();
               searchQueue.pop_back();
               auto result = visited.insert(el);
               bool inserted = result.second;
               if (inserted)
               {
                  res.back().insert(el);
                  for (int n : get<Element>(el).adjacent())
                  {
                     if (p(get<Element>(el), get<Element>(n)))
                     {
                        searchQueue.push_back(n);
                     }
                  }
               }
            }
         }
      }
      return res;
   }

#ifdef RODIN_USE_MPI
   Mesh<Traits::Parallel>
   Mesh<Traits::Serial>::parallelize(boost::mpi::communicator comm)
   {
      return Mesh<Traits::Parallel>(comm, *this);
   }
#endif

   // ---- Mesh<Serial> ------------------------------------------------------
   Mesh<Traits::Serial>::Mesh(mfem::Mesh&& mesh)
      : m_mesh(std::move(mesh))
   {}

   Mesh<Traits::Serial>::Mesh(const Mesh& other)
      : m_mesh(other.m_mesh)
   {}

   Mesh<Traits::Serial>&
   Mesh<Traits::Serial>::load(const boost::filesystem::path& filename, IO::FileFormat fmt)
   {
      mfem::named_ifgzstream input(filename.c_str());
      if (!input)
      {
         Alert::Exception()
            << "Failed to open " << filename << " for reading."
            << Alert::Raise;
      }
      switch (fmt)
      {
         case IO::FileFormat::MFEM:
         {
            IO::MeshLoader<IO::FileFormat::MFEM, Traits::Serial> loader(*this);
            loader.load(input);
            break;
         }
         case IO::FileFormat::GMSH:
         {
            IO::MeshLoader<IO::FileFormat::GMSH, Traits::Serial> loader(*this);
            loader.load(input);
            break;
         }
         case IO::FileFormat::MEDIT:
         {
            IO::MeshLoader<IO::FileFormat::MEDIT, Traits::Serial> loader(*this);
            loader.load(input);
            break;
         }
         default:
         {
            Alert::Exception()
               << "Loading from \"" << fmt << "\" format unssuported."
               << Alert::Raise;
         }
      }

      return *this;
   }

   SubMesh<Traits::Serial> Mesh<Traits::Serial>::extract(const std::set<int>& elements)
   {
      SubMesh<Traits::Serial> res(*this);
      res.initialize(getDimension(), getSpaceDimension(), elements.size());
      for (int el : elements)
         res.add(get<Element>(el));
      res.finalize();
      return res;
   }

   SubMesh<Traits::Serial> Mesh<Traits::Serial>::keep(int attr)
   {
      return keep(std::set<int>{attr});
   }

   SubMesh<Traits::Serial> Mesh<Traits::Serial>::keep(const std::set<int>& attrs)
   {
      assert(!getHandle().GetNodes()); // Curved mesh or discontinuous mesh not handled yet!

      SubMesh<Traits::Serial> res(*this);
      res.initialize(getDimension(), getSpaceDimension());

      // Add elements with matching attribute
      for (int i = 0; i < count<Element>(); i++)
      {
         const auto& el = get<Element>(i);
         if (attrs.count(el.getAttribute()))
            res.add(el);
      }

      // Add the boundary elements
      for (int i = 0; i < count<BoundaryElement>(); i++)
      {
         const auto& be = get<BoundaryElement>(i);
         const auto& elems = be.elements();
         for (const auto& el : elems)
         {
            if (attrs.count(get<Element>(el).getAttribute()))
            {
               res.add(be);
               break;
            }
         }
      }
      res.finalize();
      return res;
   }

   SubMesh<Traits::Serial> Mesh<Traits::Serial>::skin()
   {
      assert(!getHandle().GetNodes()); // Curved mesh or discontinuous mesh not handled yet!

      SubMesh<Traits::Serial> res(*this);
      res.initialize(getSpaceDimension() - 1, getSpaceDimension());
      for (int i = 0; i < count<BoundaryElement>(); i++)
         res.add(get<BoundaryElement>(i));
      res.finalize();
      return res;
   }

   SubMesh<Traits::Serial> Mesh<Traits::Serial>::trim(int attr)
   {
      return trim(std::set<int>{attr});
   }

   SubMesh<Traits::Serial> Mesh<Traits::Serial>::trim(const std::set<int>& attrs)
   {
      std::set<int> complement = getAttributes();
      for (const auto& a : attrs)
         complement.erase(a);
      return keep(complement);
   }

   Mesh<Traits::Serial>& Mesh<Traits::Serial>::trace(
         const std::map<std::set<int>, int>& boundaries)
   {
      for (int i = 0; i < count<Face>(); i++)
      {
         const auto& fc = get<Face>(i);
         const auto& elems = fc.elements();
         if (elems.size() == 2)
         {
            std::set<int> k{
               get<Element>(*elems.begin()).getAttribute(),
               get<Element>(*std::next(elems.begin())).getAttribute()
            };
            auto it = boundaries.find(k);
            if (it != boundaries.end())
            {
               mfem::Element* be =
                  getHandle().NewElement(fc.getHandle().GetGeometryType());
               be->SetVertices(fc.getHandle().GetVertices());
               be->SetAttribute(it->second);
               getHandle().AddBdrElement(be);
            }
         }
      }
      getHandle().SetAttributes();
      return *this;
   }

   mfem::Mesh& Mesh<Traits::Serial>::getHandle()
   {
      return m_mesh;
   }

   const mfem::Mesh& Mesh<Traits::Serial>::getHandle() const
   {
      return m_mesh;
   }

   // ---- Mesh<Parallel> ----------------------------------------------------
}

