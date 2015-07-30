#ifndef EVENT_H
#define EVENT_H

#include "EventManager.h"

namespace genesis {
/*
////////////////////////////////////////////////////////////////////////////////
// This message is used to trigger the complete shutdown of the system        //
////////////////////////////////////////////////////////////////////////////////
class QuitMessage : public genesis::BaseEventData {
public:
	static const genesis::EventType sk_EventType;

	QuitMessage() {	}
	~QuitMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const std::string getName() const { return "QuitMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new QuitMessage()); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}
};

////////////////////////////////////////////////////////////////////////////////
// This message is used by the Display system to notify other systems when    //
// the Window has been created. This is necessary for systems that need the   //
// window handle in order to initialize themselves.                           //
////////////////////////////////////////////////////////////////////////////////
class OgreWindowCreatedMessage : public genesis::BaseEventData {
private:
	Ogre::RenderWindow*				m_pWindow;
	const size_t					m_WindowHandle;
	const unsigned int				m_WindowWidth;
	const unsigned int				m_WindowHeight;

public:
	static const genesis::EventType sk_EventType;

	OgreWindowCreatedMessage( Ogre::RenderWindow* window, size_t handle, unsigned int width, unsigned int height ) : m_pWindow(window), m_WindowHandle(handle), m_WindowWidth(width), m_WindowHeight(height) {	}
	~OgreWindowCreatedMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "OgreWindowCreatedMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new OgreWindowCreatedMessage(m_pWindow, m_WindowHandle, m_WindowWidth, m_WindowHeight)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const Ogre::RenderWindow* getRenderWindow() { return m_pWindow; }
	const size_t getWindowHandle() { return m_WindowHandle;	}
	const unsigned int getWindowWidth() { return m_WindowWidth; }
	const unsigned int getWindowHeight() { return m_WindowHeight; }
};

////////////////////////////////////////////////////////////////////////////////
// This message is used by the Display system to notify other systems when    //
// the Window size has been changed.                                          //
////////////////////////////////////////////////////////////////////////////////
class WindowResizeMessage : public genesis::BaseEventData {
private:
	const unsigned int				m_WindowWidth;
	const unsigned int				m_WindowHeight;

public:
	static const genesis::EventType sk_EventType;

	WindowResizeMessage( unsigned int width, unsigned int height ) : m_WindowWidth(width), m_WindowHeight(height) {	}
	~WindowResizeMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "WindowResizeMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new WindowResizeMessage(m_WindowWidth, m_WindowHeight)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const unsigned int getWindowWidth() { return m_WindowWidth; }
	const unsigned int getWindowHeight() { return m_WindowHeight; }
};

////////////////////////////////////////////////////////////////////////////////
// These messages are sent out by the Input system to inform other            //
// systems about user input.                                                  //
////////////////////////////////////////////////////////////////////////////////
class KeyDownMessage : public genesis::BaseEventData {
private:
	const genesis::GenesisKeyCode	m_Key;
	const unsigned int				m_Text;

public:
	static const genesis::EventType sk_EventType;

	KeyDownMessage( genesis::GenesisKeyCode key, unsigned int text ) : m_Key(key), m_Text(text) { }
	~KeyDownMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "KeyDownMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new KeyDownMessage(m_Key, m_Text)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const genesis::GenesisKeyCode getKey() { return m_Key; }
	const unsigned int getText() { return m_Text; }
};

class KeyUpMessage : public genesis::BaseEventData {
private:
	const genesis::GenesisKeyCode	m_Key;
	const unsigned int				m_Text;

public:
	static const genesis::EventType sk_EventType;

	KeyUpMessage( genesis::GenesisKeyCode key, unsigned int text ) : m_Key(key), m_Text(text) { }
	~KeyUpMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "KeyUpMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new KeyUpMessage(m_Key, m_Text)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const genesis::GenesisKeyCode getKey() { return m_Key; }
	const unsigned int getText() { return m_Text; }
};

class MouseMovementMessage : public genesis::BaseEventData {
private:
	const int		m_Xabs, m_Xrel;
	const int		m_Yabs, m_Yrel;
	const int		m_Zabs, m_Zrel;

public:
	static const genesis::EventType sk_EventType;

	MouseMovementMessage( int xabs, int yabs, int zabs, int xrel, int yrel, int zrel ) : m_Xabs(xabs), m_Yabs(yabs), m_Zabs(zabs),
																							m_Xrel(xrel), m_Yrel(yrel), m_Zrel(zrel) { }
	~MouseMovementMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "MouseMovementMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new MouseMovementMessage(m_Xabs, m_Yabs, m_Zabs, m_Xrel, m_Yrel, m_Zrel)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const int getXabs() { return m_Xabs;}
	const int getYabs() { return m_Yabs;}
	const int getZabs() { return m_Zabs;}
	const int getXrel() { return m_Xrel;}
	const int getYrel() { return m_Yrel;}
	const int getZrel() { return m_Zrel;}
};

class MouseButtonDownMessage : public genesis::BaseEventData {
private:
	const genesis::GenesisMouseButtonID	m_Button;

public:
	static const genesis::EventType sk_EventType;

	MouseButtonDownMessage( genesis::GenesisMouseButtonID button ) : m_Button(button) { }
	~MouseButtonDownMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "MouseButtonDownMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new MouseButtonDownMessage(m_Button)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const genesis::GenesisMouseButtonID getButton() { return m_Button; }
};

class MouseButtonUpMessage : public genesis::BaseEventData {
private:
	const genesis::GenesisMouseButtonID	m_Button;

public:
	static const genesis::EventType sk_EventType;

	MouseButtonUpMessage( genesis::GenesisMouseButtonID button ) : m_Button(button) { }
	~MouseButtonUpMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "MouseButtonUpMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new MouseButtonUpMessage(m_Button)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const genesis::GenesisMouseButtonID getButton() { return m_Button; }
};

////////////////////////////////////////////////////////////////////////////////
// These messages are used to communicate with the Graphics System.           //
////////////////////////////////////////////////////////////////////////////////
class NewSceneManagerMessage : public genesis::BaseEventData {
private:
	OgreSceneType			m_SceneType;
	String					m_SceneName;

public:
	static const genesis::EventType sk_EventType;

	NewSceneManagerMessage( OgreSceneType sceneType, String sceneName ) : m_SceneType(sceneType), m_SceneName(sceneName) { }
	~NewSceneManagerMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "NewSceneManagerMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new NewSceneManagerMessage( m_SceneType, m_SceneName)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const OgreSceneType getSceneType() { return  m_SceneType; }
	const String getSceneName() { return m_SceneName; }
};

class NewCameraAndViewportMessage : public genesis::BaseEventData {
private:
	String			m_CameraName;
	float			m_CameraX;
	float			m_CameraY;
	float			m_CameraZ;
	viewportstat	m_VPS;

public:
	static const genesis::EventType sk_EventType;

	NewCameraAndViewportMessage( String cameraName, float camX, float camY, float camZ, viewportstat vps ) : m_CameraName(cameraName), m_CameraX(camX),
																											m_CameraY(camY), m_CameraZ(camZ), m_VPS(vps) { }
	~NewCameraAndViewportMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "NewCameraAndViewportMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new NewCameraAndViewportMessage(m_CameraName, m_CameraX, m_CameraY, m_CameraZ, m_VPS)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const String getCameraName() { return m_CameraName; }
	const float getCameraX() { return m_CameraX; }
	const float getCameraY() { return m_CameraY; }
	const float getCameraZ() { return m_CameraZ; }
	const viewportstat getVPS() { return m_VPS; }
};

class AddGraphicalObjectMessage : public genesis::BaseEventData {
private:
	std::shared_ptr<IObject>	m_pNewObject;
	std::shared_ptr<IObject>	m_pParentObject;

public:
	static const genesis::EventType sk_EventType;

	AddGraphicalObjectMessage( std::shared_ptr<IObject> object, std::shared_ptr<IObject> parent ) : m_pNewObject(object), m_pParentObject(parent) { }
	~AddGraphicalObjectMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "AddGraphicalObjectMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new AddGraphicalObjectMessage(m_pNewObject, m_pParentObject)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const std::shared_ptr<IObject> getNewObject() { return m_pNewObject; }
	const std::shared_ptr<IObject> getParentObject() { return m_pParentObject; }
};

class RemoveGraphicalObjectMessage : public genesis::BaseEventData {
private:
	std::shared_ptr<IObject>	m_pObject;

public:
	static const genesis::EventType sk_EventType;

	RemoveGraphicalObjectMessage( std::shared_ptr<IObject> object ) : m_pObject(object) { }
	~RemoveGraphicalObjectMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "RemoveGraphicalObjectMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new RemoveGraphicalObjectMessage(m_pObject)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const std::shared_ptr<IObject> getNewObject() { return m_pObject; }
};

////////////////////////////////////////////////////////////////////////////////
// These messages are used to communicate with the Graphics System.           //
////////////////////////////////////////////////////////////////////////////////
class CreateGUIWindowMessage : public genesis::BaseEventData {
private:
	guiwindowproperties		m_Properties;

public:
	static const genesis::EventType sk_EventType;

	CreateGUIWindowMessage( guiwindowproperties properties ) : m_Properties(properties) { }
	~CreateGUIWindowMessage() { }

	virtual const genesis::EventType& getEventType() const { return sk_EventType; }
	virtual const genesis::String getName() const { return "CreateGUIWindowMessage"; }
	virtual genesis::IEventDataPtr copy() const { return genesis::IEventDataPtr(new CreateGUIWindowMessage(m_Properties)); }
	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}

	const guiwindowproperties getProperties() { return m_Properties; }
};
*/
}

#endif /* EVENT_H */
