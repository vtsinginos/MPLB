/**
 * Copyright 2019 United Kingdom Research and Innovation
 *
 * Authors: See AUTHORS
 *
 * Contact: [jianping.meng@stfc.ac.uk and/or jpmeng@gmail.com]
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice
 *    this list of conditions and the following disclaimer in the documentation
 *    and or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * ANDANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @brief   Functions for data storage of particle mapping data per component.
 * @author  C. Tsigginos
 */

#include "mapping_particles.h"

MappingParticles::MappingParticles(int spacedim, int noElements,
		ParticleShapeDiscriptor particleShape, ParticleMappingModel mappingMod,
		std::vector<std::string> nameOfRealVars, std::vector<int> sizeRealVars,
		std::vector<std::string> nameOfIntVars,  std::vector<int> sizeIntVars,
		SizeType timestep) {

	spaceDim = spacedim;
	particleDiscriptor = particleShape;
	mappingModel = mappingMod;
	noElem = noElements;

	//Sanity checks
	if ((mappingModel == spherical) && (particleDiscriptor != spherical)) {
		ops_printf("ERROR: Analytical mapping model does not support this type of particle shape\n");
		exit(EXIT_FAILURE);
	}



	if (sizeRealVars.size() != nameOfRealVars.size()) {
		ops_printf("ERROR: Inconsistent sizes for Real ops_dat variables.\n");
		exit(EXIT_FAILURE);
	}

	for (int iVar = 0; iVar < nameOfRealVars.size(); iVar++) {
		RealField variable{nameOfRealVars.at(iVar), sizeRealVars.at(iVar)};

		mappingRealVariableList.push_back(variable);
		nameRealVariables.push_back(nameOfRealVars.at(iVar));
		mappingRealSize.push_back(sizeRealVars.at(iVar));
	}

	for (auto& mappingVariable : mappingRealVariableList) {
		if (timeStep == 0)
			mappingVariable.CreateFieldFromScratch(g_Block());
		else
			mappingVariable.CreateFieldFromFile(CaseName(), g_Block(), timestep);
	}

	//Creating integer mapping variables
	if (sizeIntVars.size() != nameOfIntVars.size()) {
			ops_printf("ERROR: Inconsistent sizes for int ops_dat variables.\n");
			exit(EXIT_FAILURE);
	}

	for (int iVar = 0; iVar < nameOfIntVars.size(); iVar++) {
		IntField variable{nameOfIntVars.at(iVar),  sizeIntVars.at(iVar)};
		mappingIntVariableList.push_back(variable);
		nameIntVariables.push_back(nameOfIntVars.at(iVar));
		mappingIntSize.push_back(sizeIntVars.at(iVar));

	}

	for (auto& mappingVariable : mappingIntVariableList) {
		if (timeStep == 0)
			mappingVariable.second.CreateFieldFromScratch(g_Block());
		else
			mappingVariable.second.CreateFieldFromFile(CaseName(), g_Block(), timestep);
	}
}

ops_dat&  MappingParticles::GetRealField(int variable, int blockIndex) {

	return mappingRealVariableList.at(variable).at(blockIndex);

}
ops_dat& MappingParticles::GetIntField(int variable, int blockIndex) {

	return mappingIntVariableList.at(variable).at(blockIndex);
}

void MappingParticles::WriteToHdf5(const std::string& caseName, TypeSize timeStep) {

	for (const auto& variableReal : mappingRealVariableList)
		variableReal.WriteToHDF5(caseName, timeStep);

	for (const auto& variableInt : mappingIntVariableList)
		variableInt.WriteToHDf5(caseName, timeStep);


}