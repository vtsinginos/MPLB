/*
 * fpi_data.h
 *
 *  Created on: Jul 7, 2021
 *      Author: jpd38567
 */

#ifndef NOP_FPI_DATA_H_
#define NOP_FPI_DATA_H_

#include "flowfield.h"
#include "type.h"
#include "dem_data.h"
#include <memory>
#include <vector>
#include "block.h"
#include "model.h"

class FpiData {

	private:
		Component compo;
		int spaceDim;
		int noElem;
		std::vector<Real> userVariables;
		bool ownedPostVelocity;
		bool ownedForceModel;
		bool ownedCollisionModel;
		bool ownedPreCollision;
		bool ownedPostStreaming;
		bool ownedDragForce;
		bool ownedInitialize;
		bool thermalFlag;
		FluidParticleModel model;

		std::vector<RealField> mappingRealVariableList;
		std::vector<IntField> mappingIntVariableList;

		std::vector<int> mappingIntSize;
		std::vector<int> mappingRealSize;

		std::vector<std::string> nameRealVariables;
		std::vector<std::string> nameIntVariables;


	public:
		FpiData(Component& compoUser, int spacdim, FluidParticleModel model, int noelem,
				std::vector<std::string> nameOfRealVars, std::vector<int> sizeRealVars,
				std::vector<std::string> nameOfIntVars,  std::vector<int> sizeIntVars,
				std::vector<bool> flagParameters, std::vector<Real> variables,
				SizeType timeStep = 0);

		Component& FluidComponent() {return compo;};
		bool OwnedPostVelocity() {return ownedPostVelocity;};
		bool OwnedForceModel() {return ownedForceModel;};
		bool OwnedPreCollision() {return ownedPreCollision;};
		bool OwnedCollisionModel() {return ownedCollisionModel;};
		bool OwnedPostStreaming() {return ownedPostStreaming;};
		bool OwnedDragForce() {return ownedDragForce;};
		bool OwnedInitialize() {return ownedInitialize;};
		bool ThermalFlag() {return thermalFlag;};
		int GetNElem() {return noElem;};
		int GetSpaceDim() {return spaceDim;};
		FluidParticleModel GetModel() {return model;};
		ops_dat&  GetRealField(int variable, int blockIndex);
		ops_dat&  GetIntField(int variable, int blockIndex);
		std::string ReturnRealVariablesName(int variable) {return nameRealVariables.at(variable);};
		std::string ReturnIntVariablesName(int variable) {return nameIntVariables.at(variable); };
		int  SizeAtIntType(int variable) { return mappingIntSize.at(variable);};
		int  SizeAtRealType(int variable) { return mappingRealSize.at(variable);};
		Real GetRealVariableAt(int elem) { return userVariables.at(elem);};
		void WriteToHdf5(const std::string& caseName, const SizeType timeStep);
};

extern std::map<int, std::shared_ptr<FpiData>> fpiModels;

#endif /* APPS_LBM_DEM_NOP_FPI_DATA_H_ */
