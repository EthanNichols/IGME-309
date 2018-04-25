#include "MyEntity.h"

#include "MyCollider.h"

using namespace Simplex;
std::map<String, MyEntity*> MyEntity::m_IDMap;
//  Accessors
matrix4 Simplex::MyEntity::GetModelMatrix(void){ return m_m4ToWorld; }
void Simplex::MyEntity::SetModelMatrix(matrix4 a_m4ToWorld)
{
	if (!m_bInMemory)
		return;

	m_m4ToWorld = a_m4ToWorld;
	m_pModel->SetModelMatrix(m_m4ToWorld);
	m_collider.SetModelMatrix(m_m4ToWorld);
}
Model* Simplex::MyEntity::GetModel(void){return m_pModel;}
Collider Simplex::MyEntity::GetCollider(void){	return m_collider; }
bool Simplex::MyEntity::IsInitialized(void){ return m_bInMemory; }
String Simplex::MyEntity::GetUniqueID(void) { return m_sUniqueID; }
void Simplex::MyEntity::SetAxisVisible(bool a_bSetAxis) { m_bSetAxis = a_bSetAxis; }
glm::vec3 Simplex::MyEntity::GetHalfSize() {
    return m_v3HalfSize;
}
//  MyEntity
void Simplex::MyEntity::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bInMemory = false;
	m_bSetAxis = false;
	m_pModel = nullptr;
	m_m4ToWorld = IDENTITY_M4;
	m_sUniqueID = "";
}
void Simplex::MyEntity::Swap(MyEntity& other)
{
	m_bInMemory = false;
	std::swap(m_pModel, other.m_pModel);
	std::swap(m_collider, other.m_collider);
	std::swap(m_m4ToWorld, other.m_m4ToWorld);
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bInMemory, other.m_bInMemory);
	std::swap(m_sUniqueID, other.m_sUniqueID);
	std::swap(m_bSetAxis, other.m_bSetAxis);
}
void Simplex::MyEntity::Release(void)
{
	m_pMeshMngr = nullptr;
	//it is not the job of the entity to release the model, 
	//it is for the mesh manager to do so.
	m_pModel = nullptr; 
	m_IDMap.erase(m_sUniqueID);
}
//The big 3
Simplex::MyEntity::MyEntity(String a_sFileName, Collider collider, String a_sUniqueID)
    : m_collider(collider)
{
	Init();
	m_pModel = new Model();
	m_pModel->Load(a_sFileName);
	//if the model is loaded
	if (m_pModel->GetName() != "")
	{
		GenUniqueID(a_sUniqueID);
		m_sUniqueID = a_sUniqueID;
		m_IDMap[a_sUniqueID] = this;
		m_bInMemory = true; //mark this entity as viable
	}
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::min());
    for (auto &pt : m_pModel->GetVertexList()) {
        if (pt.x > max.x) max.x = pt.x;
        if (pt.y > max.y) max.y = pt.y;
        if (pt.z > max.z) max.z = pt.z;
        if (pt.x < min.x) min.x = pt.x;
        if (pt.y < min.y) min.y = pt.y;
        if (pt.z < min.z) min.z = pt.z;
    }
    glm::vec3 avg = (min + max) / 2.0f;
    m_v3HalfSize = max - avg;
}
Simplex::MyEntity::MyEntity(MyEntity const& other)
    : m_collider(other.m_collider)
{
	m_bInMemory = other.m_bInMemory;
	m_pModel = other.m_pModel;
	//generate a new rigid body we do not share the same rigid body as we do the model
	m_m4ToWorld = other.m_m4ToWorld;
	m_pMeshMngr = other.m_pMeshMngr;
	m_sUniqueID = other.m_sUniqueID;
	m_bSetAxis = other.m_bSetAxis;
}
MyEntity& Simplex::MyEntity::operator=(MyEntity const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyEntity temp(other);
		Swap(temp);
	}
	return *this;
}
MyEntity::~MyEntity(){Release();}
//--- Methods
void Simplex::MyEntity::AddToRenderList(bool a_bDrawCollider)
{
	//if not in memory return
	if (!m_bInMemory)
		return;

	//draw model
	m_pModel->AddToRenderList();
	
	//draw rigid body
	if(a_bDrawCollider)
		m_collider.AddToRenderList();

	if (m_bSetAxis)
		m_pMeshMngr->AddAxisToRenderList(m_m4ToWorld);
}
bool Simplex::MyEntity::IsColliding(MyEntity* const other)
{
	//if not in memory return
	if (!m_bInMemory || !other->m_bInMemory)
		return true;

	return m_collider.IsColliding(other->GetCollider());
}
MyEntity* Simplex::MyEntity::GetEntity(String a_sUniqueID)
{
	//look the entity based on the unique id
	auto entity = m_IDMap.find(a_sUniqueID);
	//if not found return nullptr, if found return it
	return entity == m_IDMap.end() ? nullptr : entity->second;
}
void Simplex::MyEntity::GenUniqueID(String& a_sUniqueID)
{
	static uint index = 0;
	String sName = a_sUniqueID;
	MyEntity* pEntity = GetEntity(a_sUniqueID);
	//while Entity exists keep changing name
	while (pEntity)
	{
		a_sUniqueID = sName + "_" + std::to_string(index);
		index++;
		pEntity = GetEntity(a_sUniqueID);
	}
	return;
}