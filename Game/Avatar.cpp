#include "pch.h"
#include "Avatar.h"
#include "Texture.h"
#include "utils.h"
#include "Level.h"
#include "ProjectileManager.h"
#include "SoundManager.h"
#include "TextureManager.h"

Avatar::Avatar(SoundManager* SM, TextureManager* TM)
	: m_Slashing{false}
	, m_JumpSpeed{300}
	, m_WalkSpeed{90}
	, m_Velocity{0,0}
	, m_FallAccel{ -800 }
	, m_CurrentState{ m_ActionState::idle }
	, m_TextureStr{"resources/images/ryu.png"}
	, m_AnimFrame{0}
	, m_AnimTime{0.f}
	, m_FacingRight{true}
	, m_srcRect{}
	, m_ChangedDirectionMidAir{false}
	, m_NrOfFrames{3}
	, m_SecondsInAction{0.f}
	, m_NrFramesPerSec{0}
	, m_HP{16}
	, m_Untargetable{false}
	, m_SecondsUntargetable{0.f}
	, m_Score{0}
	, m_Lives{2}
	, m_Energy{0}
	, m_Powerup{0}
	, m_InfFireWheelStr{"resources/images/infinitefirewheel.png"}
	, m_JumpSlashingStr{"resources/images/jumpslashing.png"}
	, m_SecWInvincibleFireWheel{-0.1f}
	, m_SecWInvFireWheelInteger{0}
	, m_FireWheelAnimTime{0.f}
	, m_FireWheelAnimFrame{0}
	, m_SlashSoundStr{"resources/sounds/soundeffects/slash.wav"}
	, m_JumpSoundStr{"Resources/Sounds/SoundEffects/jumpSound.wav"}
	, m_DamageSoundStr{"resources/sounds/soundeffects/takedamage.wav"}
	, m_ThrowStarSoundStr{"resources/sounds/soundeffects/throwstar.wav"}
	, m_JumpSlashSoundStr{"resources/sounds/soundeffects/jumpslash.wav"}
	, m_FireWheelSoundStr{ "resources/sounds/soundeffects/firewheel.wav" }
	, m_InfFireWheelSoundStr{ "resources/sounds/soundeffects/infinitefirewheel.wav" }
	, m_WindmillStarSoundStr{ "resources/sounds/soundeffects/windmillstar.wav" }
	, m_pTM{TM}
	, m_pSM{SM}
{
	m_pTM->LoadTexture(m_TextureStr);
	m_pTM->LoadTexture(m_InfFireWheelStr);
	m_pTM->LoadTexture(m_JumpSlashingStr);

	m_pSM->LoadEffect(m_SlashSoundStr);
	m_pSM->LoadEffect(m_JumpSoundStr);
	m_pSM->LoadEffect(m_DamageSoundStr);
	m_pSM->LoadEffect(m_ThrowStarSoundStr);
	m_pSM->LoadEffect(m_JumpSlashSoundStr);
	m_pSM->LoadEffect(m_FireWheelSoundStr);
	m_pSM->LoadEffect(m_InfFireWheelSoundStr);
	m_pSM->LoadEffect(m_WindmillStarSoundStr);

	m_Shape = Rectf{	20,
						20,
						m_pTM->GetTexture(m_TextureStr)->GetWidth() / 5,
						m_pTM->GetTexture(m_TextureStr)->GetHeight() / 4};
}

Avatar::~Avatar()
{
	m_pTM->DeleteTexture(m_TextureStr);
	m_pTM->DeleteTexture(m_InfFireWheelStr);
	m_pTM->DeleteTexture(m_JumpSlashingStr);

	m_pSM->DeleteEffect(m_SlashSoundStr);
	m_pSM->DeleteEffect(m_JumpSoundStr);
	m_pSM->DeleteEffect(m_DamageSoundStr);
	m_pSM->DeleteEffect(m_ThrowStarSoundStr);
	m_pSM->DeleteEffect(m_JumpSlashSoundStr);
	m_pSM->DeleteEffect(m_FireWheelSoundStr);
	m_pSM->DeleteEffect(m_InfFireWheelSoundStr);
	m_pSM->DeleteEffect(m_WindmillStarSoundStr);
}

void Avatar::Update(float elapsedSec, const Level* pLevel)
{
	m_ActionState storedStateLastFrame{ m_CurrentState };

	if (m_CurrentState != m_ActionState::climbing) DoFalling(elapsedSec);

	if (m_SecWInvincibleFireWheel >= 0)
	{
		m_SecWInvincibleFireWheel -= elapsedSec;

		if (m_SecWInvincibleFireWheel <= m_SecWInvFireWheelInteger)
		{
			m_SecWInvFireWheelInteger--;
			m_pSM->PlayEffect(m_InfFireWheelSoundStr);
		}
	}

	// All movement per state
	switch (m_CurrentState)
	{
	case m_ActionState::idle:
	{
		if (DoMovement(pLevel)) { m_CurrentState = m_ActionState::moving; }
		if (!pLevel->HandleVerticalCollision(m_Shape, m_Velocity)) { m_CurrentState = m_ActionState::falling; }

		else
		{
			m_Velocity = Vector2f{ 0,0 };
		}
		break;
	}
	case m_ActionState::moving:
	{
		bool moving{ DoMovement(pLevel) };
		bool falling{ false };

		UpdatePos(elapsedSec);

		pLevel->HandleHorizontalCollision(m_Shape, m_Velocity);

		if (!pLevel->HandleVerticalCollision(m_Shape, m_Velocity))
		{
			falling = true;
			m_ChangedDirectionMidAir = false;
		}

		if (!moving && !falling) { m_CurrentState = m_ActionState::idle; }
		if (falling) { m_CurrentState = m_ActionState::falling; }
		break;
	}
	case m_ActionState::hurt:
	{
		UpdatePos(elapsedSec);

		if (pLevel->HandleHorizontalCollision(m_Shape, m_Velocity))
		{
			m_CurrentState = m_ActionState::climbing;
			m_FacingRight = !m_FacingRight;
			m_Velocity = Vector2f{ 0,0 };
			m_SecondsUntargetable = 0.5f;
			break;
		}

		if (pLevel->HandleVerticalCollision(m_Shape, m_Velocity))
		{
			m_CurrentState = m_ActionState::idle;
			m_SecondsUntargetable = 0.5f;
		}
		m_ChangedDirectionMidAir = false;

		break;
	}
	case m_ActionState::jumpSlashing:
	case m_ActionState::falling:
	{
		float lastFrameVelocityX{ m_Velocity.x };
		bool moving{ DoMovement(pLevel) };
		bool falling{ true };

		UpdatePos(elapsedSec);

		if (pLevel->HandleVerticalCollision(m_Shape, m_Velocity))
		{
			falling = false;
		}

		if (pLevel->HandleHorizontalCollision(m_Shape, m_Velocity))
		{
			m_CurrentState = m_ActionState::climbing;
			m_pSM->PlayEffect(m_JumpSoundStr);
			m_Velocity = Vector2f{ 0,0 };
			m_ChangedDirectionMidAir = false;
			break;
		}

		if (falling && lastFrameVelocityX != m_Velocity.x) { m_ChangedDirectionMidAir = true; }

		if (!falling)
		{
			m_CurrentState = m_ActionState::idle;
			m_ChangedDirectionMidAir = false;

			if (moving) 
			{ 
				m_CurrentState = m_ActionState::moving; 
			}
		}
		break;
	}
	case m_ActionState::crouching:
	{
		if (DoMovement(pLevel))
		{
			m_CurrentState = m_ActionState::moving;
		}
		UpdatePos(elapsedSec);

		pLevel->HandleVerticalCollision(m_Shape, m_Velocity);

		break;
	}
	case m_ActionState::usingItem:
	{
		float lastFrameVelocityX{ m_Velocity.x };
		bool facingRightLastFrame{ m_FacingRight };
		bool moving{ DoMovement(pLevel) };
		m_FacingRight = facingRightLastFrame;
		bool falling{ true };

		if (pLevel->HandleVerticalCollision(m_Shape, m_Velocity))
		{
			falling = false;
		}

		if (falling)
		{
			UpdatePos(elapsedSec);
		}

		if (pLevel->HandleHorizontalCollision(m_Shape, m_Velocity))
		{
			m_CurrentState = m_ActionState::climbing;
			m_Velocity = Vector2f{ 0,0 };
			m_ChangedDirectionMidAir = false;
			break;
		}

		if (falling && lastFrameVelocityX != m_Velocity.x) { m_ChangedDirectionMidAir = true; }

		if (!falling)
		{
			m_ChangedDirectionMidAir = false;
		}

		m_SecondsInAction += elapsedSec;
		if (m_SecondsInAction >= 0.1f)
		{
			m_CurrentState = m_ActionState::idle;
			m_SecondsInAction = 0.f;
		}
		break;
	}
	case m_ActionState::climbing:
	{
		const Uint8* pStates = SDL_GetKeyboardState(nullptr);
		if (pStates[SDL_SCANCODE_X])
		{
			Jump();
		}

		break;
	}
	case m_ActionState::attacking:
	{
		float lastFrameVelocityX{ m_Velocity.x };
		bool facingRightLastFrame{ m_FacingRight };
		bool moving{ DoMovement(pLevel) };
		m_FacingRight = facingRightLastFrame;
		bool falling{ true };

		if (pLevel->HandleVerticalCollision(m_Shape, m_Velocity))
		{
			falling = false;
		}

		if (falling)
		{
			UpdatePos(elapsedSec);
		}

		if (pLevel->HandleHorizontalCollision(m_Shape, m_Velocity))
		{
			m_CurrentState = m_ActionState::climbing;
			m_Velocity = Vector2f{ 0,0 };
			m_ChangedDirectionMidAir = false;
			break;
		}

		if (falling && lastFrameVelocityX != m_Velocity.x) { m_ChangedDirectionMidAir = true; }

		if (!falling)
		{
			m_ChangedDirectionMidAir = false;
		}

		m_SecondsInAction += elapsedSec;
		if (m_SecondsInAction >= 0.29f)
		{
			m_CurrentState = m_ActionState::idle;
			m_SecondsInAction = 0.f;
		}
		break;
	}
	case m_ActionState::crouchAttacking:
	{
		m_SecondsInAction += elapsedSec;
		if (m_SecondsInAction >= 0.29f)
		{
			m_CurrentState = m_ActionState::crouching;
			m_SecondsInAction = 0.f;
		}
		break;
	}
	}
	

	if (m_Untargetable && !(m_CurrentState == m_ActionState::hurt))
	{
		m_SecondsUntargetable -= elapsedSec;
		if (m_SecondsUntargetable <= 0)
		{
			m_Untargetable = 0;
		}
	}


	if (m_CurrentState != storedStateLastFrame) 
	{ 
		m_AnimTime = 0.f;
		m_AnimFrame = 0;
	}

	UpdateSprite(elapsedSec);
}

void Avatar::Draw() const
{
	if (m_Untargetable && !(m_CurrentState == m_ActionState::hurt))
	{
		if (int(m_SecondsUntargetable * 40) % 2 == 0)
		{
			return;
		}
	}

	if (m_SecWInvincibleFireWheel >= 0)
	{
		Texture* fireWheelTexture{ m_pTM->GetTexture(m_InfFireWheelStr) };
		Rectf fireWheelRect{ m_Shape.left + m_Shape.width / 2 - fireWheelTexture->GetWidth() / 8,
							m_Shape.bottom + m_Shape.height / 2 - fireWheelTexture->GetHeight() / 2,
							fireWheelTexture->GetWidth()/4,
							fireWheelTexture->GetHeight() };

		Rectf srcRect{ 0, 0, fireWheelRect.width, fireWheelRect.height };
		srcRect.left = m_FireWheelAnimFrame * srcRect.width;

		fireWheelTexture->Draw(fireWheelRect, srcRect);
	}

	// Made a variable because it is used a lot in the next switch
	Texture* avatarTexture{ m_pTM->GetTexture(m_TextureStr) };

	Rectf srcRect;
	srcRect.width = 22;
	srcRect.height = 32;

	glPushMatrix();
	if (!m_FacingRight)
	{
		glTranslatef(2 * m_Shape.left + m_Shape.width, 0, 0);
		glScalef(-1, 1, 1);
	}

	switch (m_CurrentState)
	{
	case m_ActionState::idle:
	{
		srcRect.left = 0;
		srcRect.bottom = 32;
		avatarTexture->Draw(m_Shape, srcRect);
		break;
	}
	case m_ActionState::moving:
	{
		srcRect.left = srcRect.width * (1 + m_AnimFrame);
		srcRect.bottom = srcRect.height;

		avatarTexture->Draw(m_Shape, srcRect);
		break;
	}
	case m_ActionState::hurt:
	{
		srcRect.left = srcRect.width * 4;
		srcRect.bottom = srcRect.height;

		avatarTexture->Draw(m_Shape, srcRect);
		break;
	}
	case m_ActionState::falling:
	{
		srcRect.left = srcRect.width * 4;
		srcRect.bottom = srcRect.height;

		srcRect.height -= 8;
		srcRect.bottom += 1;
		// Center translation
		glTranslatef(m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2, 0);
		glRotatef(-90.f * m_AnimFrame, 0, 0, 1);

		Rectf tempShape{ -srcRect.width / 2,-srcRect.height / 2, srcRect.width, srcRect.height };
		avatarTexture->Draw(tempShape, srcRect);
		break;
	}
	case m_ActionState::crouching:
	{
		srcRect.left = 0;
		srcRect.bottom = srcRect.height * 2;

		avatarTexture->Draw(m_Shape, srcRect);
		break;
	}
	case m_ActionState::usingItem:
	{
		Rectf tempShape{ m_Shape };

		srcRect.left = srcRect.width * (1 + m_AnimFrame);

		if (m_AnimFrame != 0)
		{
			tempShape.width *= 2;
			srcRect.width *= 2;
		}

		srcRect.bottom = srcRect.height * 2;

		avatarTexture->Draw(tempShape, srcRect);

		break;
	}
	case m_ActionState::climbing:
	{
		srcRect.left = srcRect.width * 4;
		srcRect.bottom = srcRect.height * 2;
		avatarTexture->Draw(m_Shape, srcRect);
		break;
	}
	case m_ActionState::attacking:
	{
		Rectf tempShape{ m_Shape };

		srcRect.left = srcRect.width * m_AnimFrame;

		if (m_AnimFrame != 0)
		{
			tempShape.width *= 2;
			srcRect.width *= 2;

			if (m_AnimFrame == 2)
			{
				srcRect.left += srcRect.width / 2.f;
			}
		}

		srcRect.bottom = srcRect.height * 3;

		avatarTexture->Draw(tempShape, srcRect);

		break;
	}
	case m_ActionState::crouchAttacking:
	{
		Rectf tempShape{ m_Shape };

		srcRect.left = srcRect.width * m_AnimFrame;

		if (m_AnimFrame != 0)
		{
			tempShape.width *= 2;
			srcRect.width *= 2;

			if (m_AnimFrame == 2)
			{
				srcRect.left += srcRect.width / 2.f;
			}

		}


		srcRect.bottom = srcRect.height * 4;

		avatarTexture->Draw(tempShape, srcRect);

		break;
	}
	case m_ActionState::jumpSlashing:
	{
		Texture* jumpSlashTexture{ m_pTM->GetTexture(m_JumpSlashingStr) };
		Rectf tempShape{ - jumpSlashTexture->GetWidth()/2,
						 - jumpSlashTexture->GetHeight()/2,
						 jumpSlashTexture->GetWidth(),
						 jumpSlashTexture->GetHeight() };

		// Center translation
		glTranslatef(m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2, 0);
		glRotatef(-90.f * m_AnimFrame, 0, 0, 1);
		
		jumpSlashTexture->Draw(tempShape);
		break;
	}
	}
	//utils::SetColor(Color4f{ 1,0,0,1 });
	//utils::FillRect(m_Shape);
	glPopMatrix();
}

Rectf Avatar::GetShape() const
{
	return m_Shape;
}

Vector2f Avatar::GetVelocity() const
{
	return m_Velocity;
}

bool Avatar::DoMovement(const Level* level)
{
	bool moving{ false };

	m_Velocity.x = 0;

	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	if (pStates[SDL_SCANCODE_RIGHT])
	{
		m_Velocity.x = m_WalkSpeed;
		if (m_ChangedDirectionMidAir) { m_Velocity.x = m_Velocity.x / 3; }
		moving = true;
		m_FacingRight = true;
	}
	else if (pStates[SDL_SCANCODE_LEFT])
	{
		m_Velocity.x = -m_WalkSpeed;
		if (m_ChangedDirectionMidAir) { m_Velocity.x = m_Velocity.x / 3; }
		moving = true;
		m_FacingRight = false;
	}
	else if (pStates[SDL_SCANCODE_DOWN])
	{
		if (m_CurrentState == m_ActionState::idle)
		{
			moving = false;
			m_CurrentState = m_ActionState::crouching;
		}
	}
	else if (m_CurrentState == m_ActionState::crouching)
	{
		m_CurrentState = m_ActionState::idle;
	}
	if (m_CurrentState == m_ActionState::crouching && pStates[SDL_SCANCODE_UP])
	{
		m_CurrentState = m_ActionState::idle;
	}


	if (pStates[SDL_SCANCODE_RIGHT] && pStates[SDL_SCANCODE_LEFT])
	{
		m_Velocity.x = 0;
		moving = false;
	}

	return moving;
}

void Avatar::DoFalling(float elapsedSec)
{
	m_Velocity.y += m_FallAccel * elapsedSec;
}

void Avatar::UpdatePos(float elapsedSec)
{
	m_Shape.bottom += m_Velocity.y * elapsedSec;
	m_Shape.left += m_Velocity.x * elapsedSec;
}

void Avatar::Jump()
{
	if (m_CurrentState == m_ActionState::climbing)
	{
		const Uint8* pStates = SDL_GetKeyboardState(nullptr);
		if (m_FacingRight && pStates[SDL_SCANCODE_LEFT])
		{
			m_Velocity.y = 3 * m_JumpSpeed / 5;
			m_Velocity.x = -m_WalkSpeed;
			m_pSM->PlayEffect(m_JumpSoundStr);
			m_CurrentState = m_ActionState::falling;
		}

		else if (!m_FacingRight && pStates[SDL_SCANCODE_RIGHT])
		{
			m_Velocity.y = 3 * m_JumpSpeed / 5;
			m_Velocity.x = m_WalkSpeed;
			m_pSM->PlayEffect(m_JumpSoundStr);
			m_CurrentState = m_ActionState::falling;
		}
	}

	else if    (m_CurrentState == m_ActionState::idle || 
				m_CurrentState == m_ActionState::moving ||
				m_CurrentState == m_ActionState::crouching)
	{
		m_Velocity.y = m_JumpSpeed;
		m_CurrentState = m_ActionState::falling;
		m_pSM->PlayEffect(m_JumpSoundStr);

		m_AnimTime = 0.f;
		m_AnimFrame = 0;
	}
}

void Avatar::UpdateSprite(float elapsedSec)
{
	switch (m_CurrentState)
	{
	case m_ActionState::idle:
	case m_ActionState::climbing:
	case m_ActionState::crouching:
	case m_ActionState::hurt:
	{
		m_NrOfFrames = 1;
		m_NrFramesPerSec = 1;
		break;
	}
	case m_ActionState::moving:
	{
		m_NrOfFrames = 3;
		m_NrFramesPerSec = 12;
		break;
	}
	case m_ActionState::jumpSlashing:
	case m_ActionState::falling:
	{
		m_NrOfFrames = 4;
		m_NrFramesPerSec = 16;
		break;
	}
	case m_ActionState::usingItem:
	{
		m_NrOfFrames = 2;
		m_NrFramesPerSec = 6;
		break;
	}
	case m_ActionState::attacking:
	case m_ActionState::crouchAttacking:
	{
		m_NrOfFrames = 3;
		m_NrFramesPerSec = 9;
		break;
	}
	}

	// General animation
	m_AnimTime += elapsedSec;
	if (m_AnimTime >= 1.f / m_NrFramesPerSec)
	{
		m_AnimTime -= 1.f / m_NrFramesPerSec;
		++m_AnimFrame %= m_NrOfFrames;
	}

	// Fire wheel
	if (m_SecWInvincibleFireWheel >= 0)
	{
		int fps{ 24 };
		int frames{ 4 };

		m_FireWheelAnimTime += elapsedSec;
		if (m_FireWheelAnimTime >= 1.f / fps)
		{
			m_FireWheelAnimTime -= 1.f / fps;
			++m_FireWheelAnimFrame %= frames;
		}
	}

}

void Avatar::Attack()
{
	if (m_CurrentState == m_ActionState::falling && m_Powerup == 7)
	{
		if (m_Energy >= 5)
		{
			m_CurrentState = m_ActionState::jumpSlashing;
			m_pSM->PlayEffect(m_JumpSlashSoundStr);
			m_Energy -= 5;
		}
	}

	if (m_CurrentState == m_ActionState::idle ||
		m_CurrentState == m_ActionState::moving ||
		m_CurrentState == m_ActionState::falling)
	{
		m_CurrentState = m_ActionState::attacking;
		m_pSM->PlayEffect(m_SlashSoundStr);
		m_SecondsInAction = 0.f;
		m_NrFramesPerSec = 6;

		m_AnimTime = 0.f;
		m_AnimFrame = 0;
	}

	else if (m_CurrentState == m_ActionState::crouching)
	{
		m_CurrentState = m_ActionState::crouchAttacking;
		m_pSM->PlayEffect(m_SlashSoundStr);
		m_SecondsInAction = 0.f;
		m_NrFramesPerSec = 6;

		m_AnimTime = 0.f;
		m_AnimFrame = 0;
	}
}

void Avatar::TakeDamage(bool fromRight) 
{
	if (!(m_CurrentState == m_ActionState::hurt) && (!m_Untargetable))
	{
		m_CurrentState = m_ActionState::hurt;
		m_Untargetable = true;
		m_Velocity = Vector2f{ 2 * m_WalkSpeed / 3, 3 * m_JumpSpeed / 4 };
		m_FacingRight = false;
		m_HP--;
		m_pSM->PlayEffect(m_DamageSoundStr);

		if (fromRight)
		{
			m_Velocity.x *= -1;
			m_FacingRight = true;
		}
	}
}

void Avatar::AddScore(int amount)
{
	m_Score += amount;
}

bool Avatar::IsAttacking() const 
{
	if (m_CurrentState == m_ActionState::attacking)
	{
		return true;
	}
	return false;
}

bool Avatar::IsCrouchAttacking() const
{
	if (m_CurrentState == m_ActionState::crouchAttacking)
	{
		return true;
	}
	return false;
}

bool Avatar::GetFacingRight() const
{
	return m_FacingRight;
}

int Avatar::GetHP() const
{
	return m_HP;
}

int Avatar::GetScore() const
{
	return m_Score;
}

int Avatar::GetLives() const
{
	return m_Lives;
}

int Avatar::GetEnergy() const
{
	return m_Energy;
}

int Avatar::GetPowerup() const
{
	return m_Powerup;
}

void Avatar::PowerupPickedUp(int power)
{
	switch (power)
	{
	case 1:
		m_Score += 500;
		break;
	case 2:
		m_Score += 1000;
		break;
	case 3:
		m_Energy += 5;
		break;	
	case 4:
		m_Energy += 10;
		break;
	case 8:
		m_SecWInvincibleFireWheel = 5.f;
		m_SecWInvFireWheelInteger = 4;
		break;
	case 10:
		break;
	default:
		m_Powerup = power;
		break;
	}
}

void Avatar::UseItem(ProjectileManager* pPManager)
{
	if (m_CurrentState == m_ActionState::idle ||
		m_CurrentState == m_ActionState::moving ||
		m_CurrentState == m_ActionState::falling)
	{
		m_CurrentState = m_ActionState::usingItem;
		m_SecondsInAction = 0.f;

		m_AnimTime = 0.f;
		m_AnimFrame = 0;


		switch (m_Powerup)
		{
		case 5:
		{
			if (m_Energy >= 3)
			{
				pPManager->RequestStar(Point2f{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2 }, m_FacingRight);
				m_Energy -= 3;
				m_pSM->PlayEffect(m_ThrowStarSoundStr);
			}
			break;
		}
		case 6:
		{
			if (m_Energy >= 5)
			{
				pPManager->RequestArtOfTheFireWheel(Point2f{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2 }, m_FacingRight);
				m_pSM->PlayEffect(m_FireWheelSoundStr);
				m_Energy -= 5;
			}
			break;
		}
		case 9:
		{
			if (m_Energy >= 5)
			{
				pPManager->RequestWindmillThrowingStar(Point2f{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2 }, m_FacingRight);
				m_pSM->PlayEffect(m_WindmillStarSoundStr);
				m_Energy -= 5;
			}
			break;
		}
		}


	}
}


bool Avatar::IsjumpSlashing() const
{
	if (m_CurrentState == m_ActionState::jumpSlashing)
	{
		return true;
	}
	return false;
}

bool Avatar::IsFireWheelOn() const
{
	if (m_SecWInvincibleFireWheel >= 0)
	{
		return true;
	}
	return false;
}

void Avatar::Reset()
{
	m_Velocity = Vector2f{ 0,0 };
	m_AnimFrame = 0;
	m_AnimTime = 0.f;
	m_FacingRight = true;
	m_SecondsInAction = 0.f;
	m_HP = 16;
	m_Untargetable = false;
	m_SecondsUntargetable = 0.f;
	m_Energy /= 2;
	m_Powerup = 0;
	m_Shape = Rectf{ 20, 20, m_pTM->GetTexture(m_TextureStr)->GetWidth() / 5, m_pTM->GetTexture(m_TextureStr)->GetHeight() / 4};
	m_Lives--;
}

bool Avatar::CheckIfDead()
{
	if (m_HP <= 0 && m_CurrentState != m_ActionState::hurt)
	{
		m_CurrentState = m_ActionState::hurt;
		return true;
	}
	return false;
}

void Avatar::FullReset()
{
	m_Slashing = false;
	m_Velocity = Vector2f{ 0,0 };
	m_CurrentState = m_ActionState::idle;
	m_AnimFrame = 0;
	m_AnimTime = 0.f;
	m_FacingRight = true;
	m_ChangedDirectionMidAir = false;
	m_SecondsInAction = 0.f;
	m_HP = 16;
	m_Untargetable = false;
	m_SecondsUntargetable = 0.f;
	m_Score = 0;
	m_Lives = 2;
	m_Energy = 0;
	m_Powerup = 0;
	m_Shape = Rectf{ 20, 20, m_pTM->GetTexture(m_TextureStr)->GetWidth() / 5, m_pTM->GetTexture(m_TextureStr)->GetHeight() / 4 };
}

void Avatar::SetPos(Point2f newPos)
{
	m_Shape.left = newPos.x;
	m_Shape.bottom = newPos.y;
}

void Avatar::Die()
{
	m_HP = 0;
}

bool Avatar::GetClimbing() const
{
	if (m_CurrentState == m_ActionState::climbing)
	{
		return true;
	}
	return false;
}
