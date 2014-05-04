#ifndef  CUBE_SCENE_DETAIL_ASSIMP_MESH_HPP
# define CUBE_SCENE_DETAIL_ASSIMP_MESH_HPP

# include "assimp.hpp"

# include <cube/gl/mesh.hpp>

# include <etc/log.hpp>

namespace cube { namespace scene { namespace detail {

	gl::mesh::MeshPtr
	assimp_mesh(aiMesh const* mesh)
	{
		ETC_LOG_SCOPE_COMPONENT("cube.scene.detail")
		auto res = std::make_shared<Mesh>();
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			ETC_TRACE.debug("Loading face", i);
			auto& face = mesh->mFaces[i];

			switch (face.mNumIndices)
			{
			case 3:
				res->mode(cube::gl::renderer::DrawMode::triangles);
				break;
			default:
				throw cube::exception::Exception{
					"Cannot handle a face with " + std::to_string(face.mNumIndices) + " vertices"
				};
			}

			if (mesh->HasPositions())
			{
				ETC_TRACE.debug("Append position");
				res->kind(ContentKind::vertex);
				for (unsigned int k = 0; k < face.mNumIndices; ++k)
				{
					auto index = face.mIndices[k];
					res->append(
						assimp_cast<ContentKind::vertex>(mesh->mVertices[index])
					);
				}
			}

			for (unsigned int tex_coord_idx = 0;
			     tex_coord_idx < 8;
			     tex_coord_idx++)
			{
				if (mesh->HasTextureCoords(tex_coord_idx))
				{
					ETC_TRACE.debug("Append tex coord", tex_coord_idx);
					ContentKind kind = ContentKind::tex_coord0 + tex_coord_idx;
					res->kind(kind);
					for (unsigned int k = 0; k < face.mNumIndices; ++k)
					{
						auto index = face.mIndices[k];
						res->append(
							assimp_cast<ContentKind::tex_coord0>(
								mesh->mTextureCoords[0][index]
							)
						);
					}
				}
				else
					break;
			}

			if (mesh->HasNormals())
			{
				ETC_TRACE.debug("Append normals");
				res->kind(ContentKind::normal);
				for (unsigned int k = 0; k < face.mNumIndices; ++k)
				{
					auto index = face.mIndices[k];
					res->append(
						assimp_cast<ContentKind::normal>(mesh->mNormals[index])
					);
				}
			}

			if (mesh->HasVertexColors(0))
			{
				ETC_TRACE.debug("Append colors");
				res->kind(ContentKind::color);
				for (unsigned int k = 0; k < face.mNumIndices; ++k)
				{
					auto index = face.mIndices[k];
					res->append(
						assimp_cast<ContentKind::color>(mesh->mColors[0][index])
					);
				}
			}
		}
		return res;
	}

}}}

#endif
