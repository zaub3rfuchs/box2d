// MIT License

// Copyright (c) 2019 Erin Catto

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "test.h"
#include "imgui/imgui.h"

extern B2_API bool g_blockSolve;

class BoxStack : public Test
{
public:

	enum
	{
		e_maxColumns = 5,
		e_maxRows = 15
	};

	BoxStack()
	{
		{
			b2BodyDef bd;
			b2Body* ground = m_world->CreateBody(&bd);

			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
			ground->CreateFixture(&shape, 0.0f);

			shape.SetTwoSided(b2Vec2(20.0f, 0.0f), b2Vec2(20.0f, 20.0f));
			ground->CreateFixture(&shape, 0.0f);
		}

		for (int32 i = 0; i < e_maxRows * e_maxColumns; ++i)
		{
			m_bodies[i] = nullptr;
		}

		m_bullet = nullptr;

		m_rowCount = 1;
		m_columnCount = 1;

		CreateStacks();
	}

	void CreateStacks()
	{
		for (int32 i = 0; i < e_maxRows * e_maxColumns; ++i)
		{
			if (m_bodies[i] != nullptr)
			{
				m_world->DestroyBody(m_bodies[i]);
				m_bodies[i] = nullptr;
			}
		}

		float xs[5] = {0.0f, -10.0f, -5.0f, 5.0f, 10.0f};

		for (int32 j = 0; j < m_columnCount; ++j)
		{
			b2PolygonShape shape;
			shape.SetAsBox(0.5f, 0.5f);

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 1.0f;

			for (int i = 0; i < m_rowCount; ++i)
			{
				b2BodyDef bd;
				bd.type = b2_dynamicBody;

				int32 n = j * m_rowCount + i;

				float x = 0.0f;
				//float x = RandomFloat(-0.02f, 0.02f);
				//float x = i % 2 == 0 ? -0.01f : 0.01f;
				bd.position.Set(xs[j] + x, 0.55f + 1.1f * i);
				b2Body* body = m_world->CreateBody(&bd);

				m_bodies[n] = body;

				body->CreateFixture(&fd);
			}
		}
	}

	void FireBullet()
	{
		if (m_bullet != nullptr)
		{
			m_world->DestroyBody(m_bullet);
			m_bullet = nullptr;
		}

		b2CircleShape shape;
		shape.m_radius = 0.25f;

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.density = 10.0f;

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(-31.0f, 5.0f);

		m_bullet = m_world->CreateBody(&bd);
		m_bullet->CreateFixture(&fd);

		m_bullet->SetLinearVelocity(b2Vec2(400.0f, 0.0f));
	}

	void UpdateUI() override
	{
		ImGui::SetNextWindowPos(ImVec2(10.0f, 100.0f));
		ImGui::SetNextWindowSize(ImVec2(240.0f, 100.0f));
		ImGui::Begin("Box Stack", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		bool changed = false;
		changed = changed || ImGui::SliderInt("rows", &m_rowCount, 1, e_maxRows);
		changed = changed || ImGui::SliderInt("columns", &m_columnCount, 1, e_maxColumns);

		if (changed)
		{
			CreateStacks();
		}

		if (ImGui::Button("Fire Bullet"))
		{
			FireBullet();
		}

		ImGui::Checkbox("Block Solve", &g_blockSolve);

		ImGui::End();
	}

	static Test* Create()
	{
		return new BoxStack;
	}

	b2Body* m_bullet;
	b2Body* m_bodies[e_maxRows * e_maxColumns];
	int32 m_columnCount;
	int32 m_rowCount;
};

static int testIndex = RegisterTest("Stacking", "Boxes", BoxStack::Create);
