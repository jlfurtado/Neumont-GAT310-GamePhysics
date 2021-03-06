#ifndef UNIFORMDATA_H
#define UNIFORMDATA_H

// Justin Furtado
// UniformData.h
// 10/28/2016
// Stores data needed to make uniform calls!

#include "GL\glew.h"
#include "Mat4.h"
#include "ExportHeader.h"
#include "ShaderProgram.h"

namespace Engine
{
	class ENGINE_SHARED UniformData
	{
		typedef void(*UniformMethod)(UniformData *pData);

	public:
		UniformData();
		UniformData(GLenum type, void *dataAddress, int dataLoc, bool log = false);
		bool PassUniform();
		void **GetUniformDataPtrPtr();
		int GetUniformDataLoc() const;
		GLenum GetType() const;

	private:
		void InitFromType();

		static void PassFloatMat4(UniformData *pData);
		static void PassInt(UniformData *pData);
		static void PassFloatVec4(UniformData *pData);
		static void PassFloatVec3(UniformData *pData);
		static void PassFloatVec2(UniformData *pData);
		static void PassFloat(UniformData *pData);
		static void PassSubroutineIndex(UniformData *pData);
		static void PassTexture(UniformData *pData);
		static void UnknownType(UniformData *pData);

		UniformMethod m_callback;
		GLenum m_uniformType;
		int m_uniformDataLoc;
		void *m_pUniformData;
		bool m_logForThis;
	};
}

#endif // ifndef UNIFORMDATA_H