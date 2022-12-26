#include "Rodin/Variational/FiniteElementSpace.h"
#include "Rodin/Variational/LinearFormIntegrator.h"
#include "Rodin/Variational/BilinearFormIntegrator.h"

#include "Native.h"

namespace Rodin::Variational::Assembly
{
   mfem::SparseMatrix
   Native<BilinearFormBase<mfem::SparseMatrix>>
   ::execute(const Input& input) const
   {
      OperatorType res(input.testFES.getSize(), input.trialFES.getSize());
      res = 0.0;

      FormLanguage::List<BilinearFormIntegratorBase> domainBFIs;
      FormLanguage::List<BilinearFormIntegratorBase> boundaryBFIs;
      FormLanguage::List<BilinearFormIntegratorBase> interfaceBFIs;

      for (const auto& bfi : input.bfis)
      {
         switch (bfi.getRegion())
         {
            case Integrator::Region::Domain:
            {
               domainBFIs.add(bfi);
               break;
            }
            case Integrator::Region::Boundary:
            {
               boundaryBFIs.add(bfi);
               break;
            }
            case Integrator::Region::Interface:
            {
               interfaceBFIs.add(bfi);
               break;
            }
         }
      }

      for (auto it = input.mesh.getElement(); !it.end(); ++it)
      {
         for (const auto& bfi : domainBFIs)
         {
            const auto& element = *it;
            if (bfi.getAttributes().size() == 0 ||
                  bfi.getAttributes().count(element.getAttribute()))
            {
               res.AddSubMatrix(
                     input.testFES.getDOFs(element),
                     input.trialFES.getDOFs(element),
                     bfi.getMatrix(element));
            }
         }
      }

      for (auto it = input.mesh.getBoundary(); !it.end(); ++it)
      {
         for (const auto& bfi : boundaryBFIs)
         {
            const auto& boundary = *it;
            if (bfi.getAttributes().size() == 0 ||
                  bfi.getAttributes().count(boundary.getAttribute()))
            {
               res.AddSubMatrix(
                     input.testFES.getDOFs(boundary),
                     input.trialFES.getDOFs(boundary),
                     bfi.getMatrix(boundary));
            }
         }
      }

      for (auto it = input.mesh.getInterface(); !it.end(); ++it)
      {
         for (const auto& bfi : interfaceBFIs)
         {
            const auto& interface = *it;
            if (bfi.getAttributes().size() == 0 ||
                  bfi.getAttributes().count(interface.getAttribute()))
            {
               res.AddSubMatrix(
                     input.testFES.getDOFs(interface),
                     input.trialFES.getDOFs(interface),
                     bfi.getMatrix(interface));
            }
         }
      }
      return res;
   }

   mfem::Vector
   Native<LinearFormBase<mfem::Vector>>
   ::execute(const Input& input) const
   {
      VectorType res(input.fes.getSize());
      res = 0.0;

      FormLanguage::List<LinearFormIntegratorBase> domainLFIs;
      FormLanguage::List<LinearFormIntegratorBase> boundaryLFIs;
      FormLanguage::List<LinearFormIntegratorBase> interfaceLFIs;

      for (const auto& lfi : input.lfis)
      {
         switch (lfi.getRegion())
         {
            case Integrator::Region::Domain:
            {
               domainLFIs.add(lfi);
               break;
            }
            case Integrator::Region::Boundary:
            {
               boundaryLFIs.add(lfi);
               break;
            }
            case Integrator::Region::Interface:
            {
               interfaceLFIs.add(lfi);
               break;
            }
         }
      }

      for (auto it = input.mesh.getElement(); !it.end(); ++it)
      {
         for (const auto& lfi : domainLFIs)
         {
            const auto& element = *it;
            if (lfi.getAttributes().size() == 0
                  || lfi.getAttributes().count(element.getAttribute()))
            {
               res.AddElementVector(input.fes.getDOFs(element), lfi.getVector(element));
            }
         }
      }

      for (auto it = input.mesh.getBoundary(); !it.end(); ++it)
      {
         for (const auto& lfi : boundaryLFIs)
         {
            const auto& element = *it;
            if (lfi.getAttributes().size() == 0
                  || lfi.getAttributes().count(element.getAttribute()))
            {
               res.AddElementVector(input.fes.getDOFs(element), lfi.getVector(element));
            }
         }
      }

      for (auto it = input.mesh.getInterface(); !it.end(); ++it)
      {
         for (const auto& lfi : interfaceLFIs)
         {
            const auto& element = *it;
            if (lfi.getAttributes().size() == 0
                  || lfi.getAttributes().count(element.getAttribute()))
            {
               res.AddElementVector(input.fes.getDOFs(element), lfi.getVector(element));
            }
         }
      }
      return res;
   }
}


