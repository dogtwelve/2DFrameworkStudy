#include <eepp/ui/uicontrol.hpp>
#include <eepp/ui/uitheme.hpp>
#include <eepp/ui/uimanager.hpp>
#include <eepp/graphics/primitives.hpp>
#include <eepp/graphics/subtexture.hpp>
#include <eepp/graphics/renderer/renderer.hpp>
#include <eepp/graphics/font.hpp>
#include <eepp/window/engine.hpp>

namespace EE { namespace UI {

UIControl * UIControl::New() {
	return eeNew( UIControl, () );
}

UIControl::UIControl() :
	mIdHash( 0 ),
	mPos( 0, 0 ),
	mRealPos( 0, 0 ),
	mSize( 0, 0 ),
	mRealSize( 0, 0 ),
	mFlags( UI_CONTROL_DEFAULT_FLAGS ),
	mData( 0 ),
	mParentCtrl( NULL ),
	mChild( NULL ),
	mChildLast( NULL ),
	mNext( NULL ),
	mPrev( NULL ),
	mSkinState( NULL ),
	mBackground( NULL ),
	mBorder( NULL ),
	mControlFlags( 0 ),
	mBlend( ALPHA_NORMAL ),
	mNumCallBacks( 0 ),
	mVisible( true ),
	mEnabled( true )
{
	if ( NULL == mParentCtrl && NULL != UIManager::instance()->getMainControl() ) {
		mParentCtrl = UIManager::instance()->getMainControl();
	}

	if ( NULL != mParentCtrl )
		mParentCtrl->childAdd( this );

	updateScreenPos();
	updateQuad();
}

UIControl::~UIControl() {
	removeSkin();
	eeSAFE_DELETE( mBackground );
	eeSAFE_DELETE( mBorder );

	childDeleteAll();

	if ( NULL != mParentCtrl )
		mParentCtrl->childRemove( this );

	if ( UIManager::instance()->getFocusControl() == this && UIManager::instance()->getMainControl() != this ) {
		UIManager::instance()->setFocusControl( UIManager::instance()->getMainControl() );
	}

	if ( UIManager::instance()->getOverControl() == this && UIManager::instance()->getMainControl() != this ) {
		UIManager::instance()->setOverControl( UIManager::instance()->getMainControl() );
	}
}

void UIControl::screenToControl( Vector2i& Pos ) const {
	UIControl * ParentLoop = mParentCtrl;

	Pos.x -= mRealPos.x;
	Pos.y -= mRealPos.y;

	while ( NULL != ParentLoop ) {
		const Vector2i& ParentPos = ParentLoop->getRealPosition();

		Pos.x -= ParentPos.x;
		Pos.y -= ParentPos.y;

		ParentLoop = ParentLoop->getParent();
	}
}

void UIControl::controlToScreen( Vector2i& Pos ) const {
	UIControl * ParentLoop = mParentCtrl;

	while ( NULL != ParentLoop ) {
		const Vector2i& ParentPos = ParentLoop->getRealPosition();

		Pos.x += ParentPos.x;
		Pos.y += ParentPos.y;

		ParentLoop = ParentLoop->getParent();
	}
}

Uint32 UIControl::getType() const {
	return UI_TYPE_CONTROL;
}

bool UIControl::isType( const Uint32& type ) const {
	return UIControl::getType() == type;
}

void UIControl::messagePost( const UIMessage * Msg ) {
	UIControl * Ctrl = this;

	while( NULL != Ctrl ) {
		if ( Ctrl->onMessage( Msg ) )
			break;

		Ctrl = Ctrl->getParent();
	}
}

Uint32 UIControl::onMessage( const UIMessage * Msg ) {
	return 0;
}

void UIControl::setInternalPosition( const Vector2i& Pos ) {
	mPos = Pos;
	mRealPos = Vector2i( Pos.x * PixelDensity::getPixelDensity(), Pos.y * PixelDensity::getPixelDensity() );
	updateScreenPos();
	updateChildsScreenPos();
}

UIControl * UIControl::setPosition( const Vector2i& Pos ) {
	setInternalPosition( Pos );
	onPositionChange();
	return this;
}

UIControl * UIControl::setPosition( const Int32& x, const Int32& y ) {
	setPosition( Vector2i( x, y ) );
	return this;
}

void UIControl::setPixelsPosition( const Vector2i& Pos ) {
	mPos = Vector2i( PixelDensity::pxToDpI( Pos.x ), PixelDensity::pxToDpI( Pos.y ) );
	mRealPos = Pos;
	updateScreenPos();
	updateChildsScreenPos();
	onPositionChange();
}

void UIControl::setPixelsPosition( const Int32& x, const Int32& y ) {
	setPixelsPosition( Vector2i( x, y ) );
}

const Vector2i& UIControl::getPosition() const {
	return mPos;
}

const Vector2i &UIControl::getRealPosition() const {
	return mRealPos;
}

void UIControl::setInternalSize( const Sizei& size ) {
	mSize = size;
	mRealSize = Sizei( size.x * PixelDensity::getPixelDensity(), size.y * PixelDensity::getPixelDensity() );
	updateCenter();
	sendCommonEvent( UIEvent::OnSizeChange );
}

void UIControl::setInternalPixelsSize( const Sizei& size ) {
	mSize = PixelDensity::pxToDpI( size );
	mRealSize = size;
	updateCenter();
	sendCommonEvent( UIEvent::OnSizeChange );
}

UIControl * UIControl::setSize( const Sizei& Size ) {
	if ( Size != mSize ) {
		Vector2i sizeChange( Size.x - mSize.x, Size.y - mSize.y );

		setInternalSize( Size );

		onSizeChange();

		if ( mFlags & UI_REPORT_SIZE_CHANGE_TO_CHILDS ) {
			sendParentSizeChange( sizeChange );
		}
	}

	return this;
}

UIControl * UIControl::setSize( const Int32& Width, const Int32& Height ) {
	setSize( Sizei( Width, Height ) );
	return this;
}

void UIControl::setPixelsSize( const Sizei & size ) {
	if ( size != mRealSize ) {
		Vector2i sizeChange( size.x - mRealSize.x, size.y - mRealSize.y );

		setInternalPixelsSize( size );

		onSizeChange();

		if ( mFlags & UI_REPORT_SIZE_CHANGE_TO_CHILDS ) {
			sendParentSizeChange( PixelDensity::pxToDpI( sizeChange ) );
		}
	}
}

void UIControl::setPixelsSize( const Int32& x, const Int32& y ) {
	setPixelsSize( Sizei( x, y ) );
}

void UIControl::setInternalWidth( const Int32& width ) {
	setInternalSize( Sizei( width, mSize.getHeight() ) );
}

void UIControl::setInternalHeight( const Int32& height ) {
	setInternalSize( Sizei( mSize.getWidth(), height ) );
}

void UIControl::setInternalPixelsWidth( const Int32& width ) {
	setInternalPixelsSize( Sizei( width, mRealSize.y ) );
}

void UIControl::setInternalPixelsHeight( const Int32& height ) {
	setInternalPixelsSize( Sizei( mRealSize.x, height ) );
}

Rect UIControl::getRect() const {
	return Rect( mPos, mSize );
}

const Sizei& UIControl::getSize() {
	return mSize;
}

const Sizei& UIControl::getRealSize() {
	return mRealSize;
}

UIControl * UIControl::setVisible( const bool& visible ) {
	if ( mVisible != visible ) {
		mVisible = visible;
		onVisibilityChange();
	}
	return this;
}

bool UIControl::isVisible() const {
	return mVisible;
}

bool UIControl::isHided() const {
	return !mVisible;
}

UIControl * UIControl::setEnabled( const bool& enabled ) {
	if ( mEnabled != enabled ) {
		mEnabled = enabled;
		onEnabledChange();
	}
	return this;
}

bool UIControl::isEnabled() const {
	return mEnabled;
}

bool UIControl::isDisabled() const {
	return !mEnabled;
}

UIControl * UIControl::getParent() const {
	return mParentCtrl;
}

UIControl * UIControl::setParent( UIControl * parent ) {
	if ( parent == mParentCtrl )
		return this;

	if ( NULL != mParentCtrl )
		mParentCtrl->childRemove( this );

	mParentCtrl = parent;

	if ( NULL != mParentCtrl )
		mParentCtrl->childAdd( this );

	updateScreenPos();

	updateChildsScreenPos();

	updateQuad();

	onParentChange();

	return this;
}

bool UIControl::isParentOf( UIControl * Ctrl ) {
	eeASSERT( NULL != Ctrl );

	UIControl * tParent = Ctrl->getParent();

	while ( NULL != tParent ) {
		if ( this == tParent )
			return true;

		tParent = tParent->getParent();
	}

	return false;
}

void UIControl::centerHorizontal() {
	UIControl * Ctrl = getParent();

	if ( NULL != Ctrl )
		setPosition( ( Ctrl->getSize().getWidth() - mSize.getWidth() ) / 2, mPos.y );
}

void UIControl::centerVertical(){
	UIControl * Ctrl = getParent();

	if ( NULL != Ctrl )
		setPosition( mPos.x, ( Ctrl->getSize().getHeight() - mSize.getHeight() ) / 2 );
}

void UIControl::center() {
	centerHorizontal();
	centerVertical();
}

void UIControl::close() {
	mControlFlags |= UI_CTRL_FLAG_CLOSE;

	UIManager::instance()->addToCloseQueue( this );
}

void UIControl::drawHighlightFocus() {
	if ( UIManager::instance()->getHighlightFocus() && UIManager::instance()->getFocusControl() == this ) {
		Primitives P;
		P.setFillMode( DRAW_LINE );
		P.setBlendMode( getBlendMode() );
		P.setColor( UIManager::instance()->getHighlightFocusColor() );
		P.setLineWidth( PixelDensity::dpToPxI( 1 ) );
		P.drawRectangle( getRectf() );
	}
}

void UIControl::drawOverControl() {
	if ( UIManager::instance()->getHighlightOver() && UIManager::instance()->getOverControl() == this ) {
		Primitives P;
		P.setFillMode( DRAW_LINE );
		P.setBlendMode( getBlendMode() );
		P.setColor( UIManager::instance()->getHighlightOverColor() );
		P.setLineWidth( PixelDensity::dpToPxI( 1 ) );
		P.drawRectangle( getRectf() );
	}
}

void UIControl::drawDebugData() {
	if ( UIManager::instance()->getDrawDebugData() ) {
		if ( isWidget() ) {
			UIWidget * me = static_cast<UIWidget*>( this );

			if ( UIManager::instance()->getOverControl() == this ) {
				String text( String::strFormated( "X: %d Y: %d\nW: %d H: %d", mPos.x, mPos.y, mSize.x, mSize.y ) );

				if ( !mId.empty() ) {
					text = "ID: " + mId + "\n" + text;
				}

				me->setTooltipText( text );
			} else {
				me->setTooltipText( "" );
			}
		}
	}
}

void UIControl::drawBox() {
	if ( UIManager::instance()->getDrawBoxes() ) {
		Primitives P;
		P.setFillMode( DRAW_LINE );
		P.setBlendMode( getBlendMode() );
		P.setColor( Color::fromPointer( this ) );
		P.setLineWidth( PixelDensity::dpToPxI( 1 ) );
		P.drawRectangle( getRectf() );
	}
}

void UIControl::drawSkin() {
	if ( NULL != mSkinState ) {
		if ( mFlags & UI_SKIN_KEEP_SIZE_ON_DRAW ) {
			Sizei rSize = PixelDensity::dpToPxI( mSkinState->getSkin()->getSize( mSkinState->getState() ) );
			Sizei diff = ( mRealSize - rSize ) / 2;

			mSkinState->draw( mScreenPosf.x + diff.x, mScreenPosf.y + diff.y, (Float)rSize.getWidth(), (Float)rSize.getHeight(), 255 );
		} else {
			mSkinState->draw( mScreenPosf.x, mScreenPosf.y, (Float)mRealSize.getWidth(), (Float)mRealSize.getHeight(), 255 );
		}
	}
}

void UIControl::draw() {
	if ( mVisible ) {
		drawBackground();

		drawBorder();

		drawSkin();

		drawDebugData();

		drawBox();

		drawHighlightFocus();

		drawOverControl();
	}
}

void UIControl::update() {
	UIControl * ChildLoop = mChild;

	while ( NULL != ChildLoop ) {
		ChildLoop->update();
		ChildLoop = ChildLoop->mNext;
	}

	if ( mControlFlags & UI_CTRL_FLAG_MOUSEOVER_ME_OR_CHILD )
		writeCtrlFlag( UI_CTRL_FLAG_MOUSEOVER_ME_OR_CHILD, 0 );
}

void UIControl::sendMouseEvent( const Uint32& Event, const Vector2i& Pos, const Uint32& Flags ) {
	UIEventMouse MouseEvent( this, Event, Pos, Flags );
	sendEvent( &MouseEvent );
}

void UIControl::sendCommonEvent( const Uint32& Event ) {
	UIEvent CommonEvent( this, Event );
	sendEvent( &CommonEvent );
}

Uint32 UIControl::onKeyDown( const UIEventKey& Event ) {
	sendEvent( &Event );
	return 0;
}

Uint32 UIControl::onKeyUp( const UIEventKey& Event ) {
	sendEvent( &Event );
	return 0;
}

Uint32 UIControl::onMouseMove( const Vector2i& Pos, const Uint32 Flags ) {
	sendMouseEvent( UIEvent::MouseMove, Pos, Flags );
	return 1;
}

Uint32 UIControl::onMouseDown( const Vector2i& Pos, const Uint32 Flags ) {
	sendMouseEvent( UIEvent::MouseDown, Pos, Flags );

	setSkinState( UISkinState::StateMouseDown );

	return 1;
}

Uint32 UIControl::onMouseUp( const Vector2i& Pos, const Uint32 Flags ) {
	sendMouseEvent( UIEvent::MouseUp, Pos, Flags );

	setPrevSkinState();

	return 1;
}

Uint32 UIControl::onMouseClick( const Vector2i& Pos, const Uint32 Flags ) {
	sendMouseEvent( UIEvent::MouseClick, Pos, Flags );
	return 1;
}

bool UIControl::isMouseOver() {
	return 0 != ( mControlFlags & UI_CTRL_FLAG_MOUSEOVER );
}

bool UIControl::isMouseOverMeOrChilds() {
	return 0 != ( mControlFlags & UI_CTRL_FLAG_MOUSEOVER_ME_OR_CHILD );
}

Uint32 UIControl::onMouseDoubleClick( const Vector2i& Pos, const Uint32 Flags ) {
	sendMouseEvent( UIEvent::MouseDoubleClick, Pos, Flags );
	return 1;
}

Uint32 UIControl::onMouseEnter( const Vector2i& Pos, const Uint32 Flags ) {
	writeCtrlFlag( UI_CTRL_FLAG_MOUSEOVER, 1 );

	sendMouseEvent( UIEvent::MouseEnter, Pos, Flags );

	setSkinState( UISkinState::StateMouseEnter );

	return 1;
}

Uint32 UIControl::onMouseExit( const Vector2i& Pos, const Uint32 Flags ) {
	writeCtrlFlag( UI_CTRL_FLAG_MOUSEOVER, 0 );

	sendMouseEvent( UIEvent::MouseExit, Pos, Flags );

	setSkinState( UISkinState::StateMouseExit );

	return 1;
}

Uint32 UIControl::onFocus() {
	mControlFlags |= UI_CTRL_FLAG_HAS_FOCUS;

	sendCommonEvent( UIEvent::OnFocus );

	setSkinState( UISkinState::StateFocus );

	return 1;
}

Uint32 UIControl::onFocusLoss() {
	mControlFlags &= ~UI_CTRL_FLAG_HAS_FOCUS;

	sendCommonEvent( UIEvent::OnFocusLoss );

	return 1;
}

void UIControl::onWidgetFocusLoss() {
	sendCommonEvent( UIEvent::OnWidgetFocusLoss );
}

bool UIControl::hasFocus() const {
	return 0 != ( mControlFlags & UI_CTRL_FLAG_HAS_FOCUS );
}

Uint32 UIControl::onValueChange() {
	sendCommonEvent( UIEvent::OnValueChange );

	return 1;
}

void UIControl::onClose() {
	sendCommonEvent( UIEvent::OnClose );
}

Uint32 UIControl::getHorizontalAlign() const {
	return mFlags & UI_HALIGN_MASK;
}

UIControl * UIControl::setHorizontalAlign( Uint32 halign ) {
	mFlags &= ~UI_HALIGN_MASK;
	mFlags |= halign & UI_HALIGN_MASK;

	onAlignChange();
	return this;
}

Uint32 UIControl::getVerticalAlign() const {
	return mFlags & UI_VALIGN_MASK;
}

UIControl * UIControl::setVerticalAlign( Uint32 valign ) {
	mFlags &= ~UI_VALIGN_MASK;
	mFlags |= valign & UI_VALIGN_MASK;

	onAlignChange();
	return this;
}

UIControl * UIControl::setGravity( Uint32 hvalign ) {
	mFlags &= ~( UI_VALIGN_MASK | UI_HALIGN_MASK );
	mFlags |= ( hvalign & ( UI_VALIGN_MASK | UI_HALIGN_MASK ) ) ;

	onAlignChange();
	return this;
}

UIBackground * UIControl::setBackgroundFillEnabled( bool enabled ) {
	writeFlag( UI_FILL_BACKGROUND, enabled ? 1 : 0 );

	if ( enabled && NULL == mBackground ) {
		mBackground = UIBackground::New();
	}

	return mBackground;
}

UIBorder * UIControl::setBorderEnabled( bool enabled ) {
	writeFlag( UI_BORDER, enabled ? 1 : 0 );

	if ( enabled && NULL == mBorder ) {
		mBorder = UIBorder::New();

		if ( NULL == mBackground ) {
			mBackground = UIBackground::New();
		}
	}

	return mBorder;
}

UIControl * UIControl::getNextControl() const {
	return mNext;
}

UIControl * UIControl::getPrevControl() const {
	return mPrev;
}

UIControl * UIControl::getNextControlLoop() const {
	if ( NULL == mNext )
		return getParent()->getFirstChild();
	else
		return mNext;
}

UIControl * UIControl::setData(const UintPtr& data ) {
	mData = data;
	return this;
}

const UintPtr& UIControl::getData() const {
	return mData;
}

const Uint32& UIControl::getFlags() const {
	return mFlags;
}

UIControl * UIControl::setFlags( const Uint32& flags ) {
	if ( NULL == mBackground && ( flags & UI_FILL_BACKGROUND ) )
		mBackground = UIBackground::New();

	if ( NULL == mBorder && ( flags & UI_BORDER ) )
		mBorder = UIBorder::New();

	if ( fontHAlignGet( flags ) || fontVAlignGet( flags ) ) {
		onAlignChange();
	}

	mFlags |= flags;

	return this;
}

UIControl * UIControl::unsetFlags(const Uint32 & flags) {
	if ( mFlags & flags )
		mFlags &= ~flags;

	if ( fontHAlignGet( flags ) || fontVAlignGet( flags ) ) {
		onAlignChange();
	}

	return this;
}

UIControl *UIControl::resetFlags( Uint32 newFlags ) {
	mFlags = newFlags;
	return this;
}

UIControl * UIControl::setBlendMode( const EE_BLEND_MODE& blend ) {
	mBlend = static_cast<Uint16> ( blend );
	return this;
}

EE_BLEND_MODE UIControl::getBlendMode() {
	return static_cast<EE_BLEND_MODE> ( mBlend );
}

void UIControl::toFront() {
	if ( NULL != mParentCtrl && mParentCtrl->mChildLast != this ) {
		mParentCtrl->childRemove( this );
		mParentCtrl->childAdd( this );
	}
}

void UIControl::toBack() {
	if ( NULL != mParentCtrl ) {
		mParentCtrl->childAddAt( this, 0 );
	}
}

void UIControl::toPosition( const Uint32& Pos ) {
	if ( NULL != mParentCtrl ) {
		mParentCtrl->childAddAt( this, Pos );
	}
}

void UIControl::onVisibilityChange() {
	sendCommonEvent( UIEvent::OnVisibleChange );
}

void UIControl::onEnabledChange() {
	if ( !isEnabled() && NULL != UIManager::instance()->getFocusControl() ) {
		if ( isChild( UIManager::instance()->getFocusControl() ) ) {
			UIManager::instance()->setFocusControl( NULL );
		}
	}

	sendCommonEvent( UIEvent::OnEnabledChange );
}

void UIControl::onPositionChange() {
	sendCommonEvent( UIEvent::OnPosChange );
}

void UIControl::onSizeChange() {
}

Rectf UIControl::getRectf() {
	return Rectf( mScreenPosf, Sizef( (Float)mRealSize.getWidth(), (Float)mRealSize.getHeight() ) );
}

void UIControl::drawBackground() {
	if ( mFlags & UI_FILL_BACKGROUND ) {
		mBackground->draw( getRectf(), 255.f );
	}
}

void UIControl::drawBorder() {
	if ( mFlags & UI_BORDER ) {
		mBorder->draw( getRectf(), 255.f, mBackground->getCorners(), ( mFlags & UI_CLIP_ENABLE ) != 0 );
	}
}

const Uint32& UIControl::getControlFlags() const {
	return mControlFlags;
}

void UIControl::setControlFlags( const Uint32& Flags ) {
	mControlFlags = Flags;
}

void UIControl::drawChilds() {
	if ( isReverseDraw() ) {
		UIControl * ChildLoop = mChildLast;

		while ( NULL != ChildLoop ) {
			if ( ChildLoop->mVisible ) {
				ChildLoop->internalDraw();
			}

			ChildLoop = ChildLoop->mPrev;
		}
	} else {
		UIControl * ChildLoop = mChild;

		while ( NULL != ChildLoop ) {
			if ( ChildLoop->mVisible ) {
				ChildLoop->internalDraw();
			}

			ChildLoop = ChildLoop->mNext;
		}
	}
}

void UIControl::internalDraw() {
	if ( mVisible ) {
		matrixSet();

		clipMe();

		draw();

		drawChilds();

		clipDisable();

		matrixUnset();
	}
}

void UIControl::clipMe() {
	if ( mFlags & UI_CLIP_ENABLE ) {
		if ( mFlags & UI_BORDER )
			UIManager::instance()->clipSmartEnable( this, mScreenPos.x, mScreenPos.y, mRealSize.getWidth(), mRealSize.getHeight() + 1 );
		else
			UIManager::instance()->clipSmartEnable( this, mScreenPos.x, mScreenPos.y, mRealSize.getWidth(), mRealSize.getHeight() );
	}
}

void UIControl::clipDisable() {
	if ( mFlags & UI_CLIP_ENABLE )
		UIManager::instance()->clipSmartDisable( this );
}

void UIControl::matrixSet() {
}

void UIControl::matrixUnset() {
}

void UIControl::childDeleteAll() {
	while( NULL != mChild ) {
		eeDelete( mChild );
	}
}

void UIControl::childAdd( UIControl * ChildCtrl ) {
	if ( NULL == mChild ) {
		mChild 		= ChildCtrl;
		mChildLast 	= ChildCtrl;
	} else {
		mChildLast->mNext 		= ChildCtrl;
		ChildCtrl->mPrev		= mChildLast;
		ChildCtrl->mNext		= NULL;
		mChildLast 				= ChildCtrl;
	}

	onChildCountChange();
}

void UIControl::childAddAt( UIControl * ChildCtrl, Uint32 Pos ) {
	eeASSERT( NULL != ChildCtrl );

	UIControl * ChildLoop = mChild;
	
	ChildCtrl->setParent( this );

	childRemove( ChildCtrl );
	ChildCtrl->mParentCtrl = this;
	
	if ( ChildLoop == NULL ) {
		mChild 				= ChildCtrl;
		mChildLast			= ChildCtrl;
		ChildCtrl->mNext 	= NULL;
		ChildCtrl->mPrev 	= NULL;
	} else {
		if( Pos == 0 ) {
			if ( NULL != mChild ) {
				mChild->mPrev		= ChildCtrl;
			}

			ChildCtrl->mNext 	= mChild;
			ChildCtrl->mPrev	= NULL;
			mChild 				= ChildCtrl;
		} else {
			Uint32 i = 0;

			while ( NULL != ChildLoop->mNext && i < Pos ) {
				ChildLoop = ChildLoop->mNext;
				i++;
			}

			UIControl * ChildTmp = ChildLoop->mNext;
			ChildLoop->mNext 	= ChildCtrl;
			ChildCtrl->mPrev 	= ChildLoop;
			ChildCtrl->mNext 	= ChildTmp;

			if ( NULL != ChildTmp ) {
				ChildTmp->mPrev = ChildCtrl;
			} else {
				mChildLast		= ChildCtrl;
			}
		}
	}

	onChildCountChange();
}

void UIControl::childRemove( UIControl * ChildCtrl ) {
	if ( ChildCtrl == mChild ) {
		mChild 			= mChild->mNext;

		if ( NULL != mChild ) {
			mChild->mPrev 	= NULL;

			if ( ChildCtrl == mChildLast )
				mChildLast		= mChild;
		} else {
			mChildLast		= NULL;
		}
	} else {
		if ( mChildLast == ChildCtrl )
			mChildLast = mChildLast->mPrev;

		ChildCtrl->mPrev->mNext = ChildCtrl->mNext;

		if ( NULL != ChildCtrl->mNext ) {
			ChildCtrl->mNext->mPrev = ChildCtrl->mPrev;
			ChildCtrl->mNext = NULL;
		}

		ChildCtrl->mPrev = NULL;
	}

	onChildCountChange();
}

void UIControl::childsCloseAll() {
	UIControl * ChildLoop = mChild;

	while ( NULL != ChildLoop ) {
		ChildLoop->close();
		ChildLoop = ChildLoop->mNext;
	}
}

std::string UIControl::getId() const {
	return mId;
}

UIControl * UIControl::setId(const std::string & id) {
	mId = id;
	mIdHash = String::hash( id );
	return this;
}

Uint32 UIControl::getIdHash() const {
	return mIdHash;
}

UIControl * UIControl::findIdHash( const Uint32& idHash ) {
	if ( mIdHash == idHash ) {
		return this;
	} else {
		UIControl * child = mChild;

		while ( NULL != child ) {
			UIControl * foundCtrl = child->findIdHash( idHash );

			if ( NULL != foundCtrl )
				return foundCtrl;

			child = child->mNext;
		}
	}

	return NULL;
}

UIControl * UIControl::find( const std::string& id ) {
	return findIdHash( String::hash( id ) );
}

bool UIControl::isChild( UIControl * ChildCtrl ) const {
	UIControl * ChildLoop = mChild;

	while ( NULL != ChildLoop ) {
		if ( ChildCtrl == ChildLoop )
			return true;

		ChildLoop = ChildLoop->mNext;
	}

	return false;
}

bool UIControl::inParentTreeOf( UIControl * Child ) const {
	UIControl * ParentLoop = Child->mParentCtrl;

	while ( NULL != ParentLoop ) {
		if ( ParentLoop == this )
			return true;

		ParentLoop = ParentLoop->mParentCtrl;
	}

	return false;
}

Uint32 UIControl::childCount() const {
	UIControl * ChildLoop = mChild;
	Uint32 Count = 0;

	while( NULL != ChildLoop ) {
		ChildLoop = ChildLoop->mNext;
		Count++;
	}

	return Count;
}

UIControl * UIControl::childAt( Uint32 Index ) const {
	UIControl * ChildLoop = mChild;

	while( NULL != ChildLoop && Index ) {
		ChildLoop = ChildLoop->mNext;
		Index--;
	}

	return ChildLoop;
}

UIControl * UIControl::childPrev( UIControl * Ctrl, bool Loop ) const {
	if ( Loop && Ctrl == mChild && NULL != mChild->mNext )
		return mChildLast;

	return Ctrl->mPrev;
}

UIControl * UIControl::childNext( UIControl * Ctrl, bool Loop ) const {
	if ( NULL == Ctrl->mNext && Loop )
		return mChild;

	return Ctrl->mNext;
}

UIControl * UIControl::getFirstChild() const {
	return mChild;
}

UIControl * UIControl::getLastChild() const {
	return mChildLast;
}

UIControl * UIControl::overFind( const Vector2f& Point ) {
	UIControl * pOver = NULL;

	if ( mEnabled && mVisible ) {
		updateQuad();

		if ( mPoly.pointInside( Point ) ) {
			writeCtrlFlag( UI_CTRL_FLAG_MOUSEOVER_ME_OR_CHILD, 1 );

			UIControl * ChildLoop = mChildLast;

			while ( NULL != ChildLoop ) {
				UIControl * ChildOver = ChildLoop->overFind( Point );

				if ( NULL != ChildOver ) {
					pOver = ChildOver;

					break; // Search from top to bottom, so the first over will be the topmost
				}

				ChildLoop = ChildLoop->mPrev;
			}


			if ( NULL == pOver )
				pOver = this;
		}
	}

	return pOver;
}

UIControl * UIControl::childGetAt( Vector2i CtrlPos, unsigned int RecursiveLevel ) {
	UIControl * Ctrl = NULL;

	for( UIControl * pLoop = mChild; NULL != pLoop && NULL == Ctrl; pLoop = pLoop->mNext )
	{
		if ( !pLoop->isVisible() )
			continue;

		if ( pLoop->getRect().contains( CtrlPos ) ) {
			if ( RecursiveLevel )
				Ctrl = childGetAt( CtrlPos - pLoop->getPosition(), RecursiveLevel - 1 );

			if ( NULL == Ctrl )
				Ctrl = pLoop;
		}
	}

	return Ctrl;
}

Uint32 UIControl::isAnimated() {
	return mControlFlags & UI_CTRL_FLAG_ANIM;
}

Uint32 UIControl::isDragable() {
	return mControlFlags & UI_CTRL_FLAG_DRAGABLE;
}

Uint32 UIControl::isWidget() {
	return mControlFlags & UI_CTRL_FLAG_WIDGET;
}

Uint32 UIControl::isClipped() {
	return mFlags & UI_CLIP_ENABLE;
}

Uint32 UIControl::isRotated() {
	return mControlFlags & UI_CTRL_FLAG_ROTATED;
}

Uint32 UIControl::isScaled() {
	return mControlFlags & UI_CTRL_FLAG_SCALED;
}

Uint32 UIControl::isFrameBuffer() {
	return mControlFlags & UI_CTRL_FLAG_FRAME_BUFFER;
}

bool UIControl::isMeOrParentTreeRotated() {
	UIControl * Ctrl = this;

	while( NULL != Ctrl ) {
		if ( Ctrl->isRotated() )
			return true;

		Ctrl = Ctrl->getParent();
	}

	return false;
}

bool UIControl::isMeOrParentTreeScaled() {
	UIControl * Ctrl = this;

	while( NULL != Ctrl ) {
		if ( Ctrl->isScaled() )
			return true;

		Ctrl = Ctrl->getParent();
	}

	return false;
}

bool UIControl::isMeOrParentTreeScaledOrRotated() {
	UIControl * Ctrl = this;

	while( NULL != Ctrl ) {
		if ( Ctrl->isScaled() || Ctrl->isRotated() )
			return true;

		Ctrl = Ctrl->getParent();
	}

	return false;
}

bool UIControl::isMeOrParentTreeScaledOrRotatedOrFrameBuffer() {
	UIControl * Ctrl = this;

	while( NULL != Ctrl ) {
		if ( Ctrl->isScaled() || Ctrl->isRotated() || Ctrl->isFrameBuffer() )
			return true;

		Ctrl = Ctrl->getParent();
	}

	return false;
}

Polygon2f& UIControl::getPolygon() {
	return mPoly;
}

const Vector2f& UIControl::getPolygonCenter() const {
	return mCenter;
}

void UIControl::updateQuad() {
	mPoly 	= Polygon2f( Rectf( mScreenPosf.x, mScreenPosf.y, mScreenPosf.x + mRealSize.getWidth(), mScreenPosf.y + mRealSize.getHeight() ) );

	UIControl * tParent = getParent();

	while ( tParent ) {
		if ( tParent->isAnimated() ) {
			UIControlAnim * tP = reinterpret_cast<UIControlAnim *> ( tParent );

			mPoly.rotate( tP->getRotation(), tP->getRotationCenter() );
			mPoly.scale( tP->getScale(), tP->getScaleCenter() );
		}

		tParent = tParent->getParent();
	};
}

void UIControl::updateCenter() {
	mCenter = Vector2f( mScreenPosf.x + (Float)mRealSize.getWidth() * 0.5f, mScreenPosf.y + (Float)mRealSize.getHeight() * 0.5f );
}

Time UIControl::getElapsed() {
	return UIManager::instance()->getElapsed();
}

Uint32 UIControl::addEventListener( const Uint32& EventType, const UIEventCallback& Callback ) {
	mNumCallBacks++;

	mEvents[ EventType ][ mNumCallBacks ] = Callback;

	return mNumCallBacks;
}

void UIControl::removeEventListener( const Uint32& CallbackId ) {
	UIEventsMap::iterator it;

	for ( it = mEvents.begin(); it != mEvents.end(); ++it ) {
		std::map<Uint32, UIEventCallback> event = it->second;

		if ( event.erase( CallbackId ) )
			break;
	}
}

void UIControl::sendEvent( const UIEvent * Event ) {
	if ( 0 != mEvents.count( Event->getEventType() ) ) {
		std::map<Uint32, UIEventCallback>			event = mEvents[ Event->getEventType() ];
		std::map<Uint32, UIEventCallback>::iterator it;

		if ( event.begin() != event.end() ) {
			for ( it = event.begin(); it != event.end(); ++it )
				it->second( Event );
		}
	}
}

UIBackground * UIControl::getBackground() {
	if ( NULL == mBackground ) {
		mBackground = UIBackground::New();
	}

	return mBackground;
}

UIBorder * UIControl::getBorder() {
	if ( NULL == mBorder ) {
		mBorder = UIBorder::New();
	}

	return mBorder;
}

void UIControl::setThemeByName( const std::string& Theme ) {
	setTheme( UIThemeManager::instance()->getByName( Theme ) );
}

void UIControl::setTheme( UITheme * Theme ) {
	setThemeSkin( Theme, "control" );
}

UIControl * UIControl::setThemeSkin( const std::string& skinName ) {
	return setThemeSkin( UIThemeManager::instance()->getDefaultTheme(), skinName );
}

UIControl * UIControl::setThemeSkin( UITheme * Theme, const std::string& skinName ) {
	if ( NULL != Theme ) {
		UISkin * tSkin = Theme->getSkin( skinName );

		if ( NULL != tSkin ) {
			Uint32 InitialState = UISkinState::StateNormal;

			if ( NULL != mSkinState ) {
				InitialState = mSkinState->getState();
			}

			removeSkin();

			mSkinState = UISkinState::New( tSkin );
			mSkinState->setState( InitialState );

			onThemeLoaded();
		}
	}

	return this;
}

UIControl * UIControl::setSkin( const UISkin& Skin ) {
	removeSkin();

	writeCtrlFlag( UI_CTRL_FLAG_SKIN_OWNER, 1 );

	UISkin * SkinCopy = const_cast<UISkin*>( &Skin )->clone();

	mSkinState = UISkinState::New( SkinCopy );

	onThemeLoaded();

	return this;
}

UIControl * UIControl::setSkin( UISkin * skin ) {
	if ( NULL != skin ) {
		if ( NULL != mSkinState && mSkinState->getSkin() == skin )
			return this;

		Uint32 InitialState = UISkinState::StateNormal;

		if ( NULL != mSkinState ) {
			InitialState = mSkinState->getState();
		}

		removeSkin();

		mSkinState = UISkinState::New( skin );
		mSkinState->setState( InitialState );

		onThemeLoaded();
	}

	return this;
}

void UIControl::removeSkin() {
	if ( NULL != mSkinState && ( mControlFlags & UI_CTRL_FLAG_SKIN_OWNER ) ) {
		UISkin * tSkin = mSkinState->getSkin();

		eeSAFE_DELETE( tSkin );
	}

	eeSAFE_DELETE( mSkinState );
}

void UIControl::onStateChange() {
}

void UIControl::onParentChange() {
}

void UIControl::onAlignChange() {
}

void UIControl::setSkinState( const Uint32& State ) {
	if ( NULL != mSkinState ) {
		mSkinState->setState( State );

		onStateChange();
	}
}

void UIControl::setPrevSkinState() {
	if ( NULL != mSkinState ) {
		mSkinState->setPrevState();

		onStateChange();
	}
}

void UIControl::setThemeToChilds( UITheme * Theme ) {
	UIControl * ChildLoop = mChild;

	while ( NULL != ChildLoop ) {
		ChildLoop->setThemeToChilds( Theme );
		ChildLoop->setTheme( Theme );	// First set the theme to childs to let the father override the childs forced themes

		ChildLoop = ChildLoop->mNext;
	}
}

void UIControl::updateChildsScreenPos() {
	UIControl * ChildLoop = mChild;

	while ( NULL != ChildLoop ) {
		ChildLoop->updateScreenPos();
		ChildLoop->updateChildsScreenPos();

		ChildLoop = ChildLoop->mNext;
	}
}

void UIControl::updateScreenPos() {
	Vector2i Pos( mRealPos );

	controlToScreen( Pos );

	mScreenPos = Pos;
	mScreenPosf = Vector2f( Pos.x, Pos.y );

	updateCenter();
}

UISkin * UIControl::getSkin() {
	if ( NULL != mSkinState )
		return mSkinState->getSkin();

	return NULL;
}

void UIControl::writeCtrlFlag( const Uint32& Flag, const Uint32& Val ) {
	BitOp::setBitFlagValue( &mControlFlags, Flag, Val );
}

void UIControl::writeFlag( const Uint32& Flag, const Uint32& Val ) {
	if ( Val )
		mFlags |= Flag;
	else {
		if ( mFlags & Flag )
			mFlags &= ~Flag;
	}
}

void UIControl::applyDefaultTheme() {
	UIThemeManager::instance()->applyDefaultTheme( this );
}

Rect UIControl::getScreenRect() {
	return Rect( mScreenPos, mRealSize );
}

Rect UIControl::makePadding( bool PadLeft, bool PadRight, bool PadTop, bool PadBottom, bool SkipFlags ) {
	Rect tPadding( 0, 0, 0, 0 );

	if ( mFlags & UI_AUTO_PADDING || SkipFlags ) {
		if ( NULL != mSkinState && NULL != mSkinState->getSkin() ) {
			Rect rPadding = mSkinState->getSkin()->getBorderSize( UISkinState::StateNormal );

			if ( PadLeft ) {
				tPadding.Left = rPadding.Left;
			}

			if ( PadRight ) {
				tPadding.Right = rPadding.Right;
			}

			if ( PadTop ) {
				tPadding.Top = rPadding.Top;
			}

			if ( PadBottom ) {
				tPadding.Bottom = rPadding.Bottom;
			}
		}
	}

	return tPadding;
}

void UIControl::setFocus() {
	UIManager::instance()->setFocusControl( this );
}

void UIControl::sendParentSizeChange( const Vector2i& SizeChange ) {
	if ( mFlags & UI_REPORT_SIZE_CHANGE_TO_CHILDS )	{
		UIControl * ChildLoop = mChild;

		while( NULL != ChildLoop ) {
			ChildLoop->onParentSizeChange( SizeChange );
			ChildLoop = ChildLoop->mNext;
		}
	}
}

void UIControl::onParentSizeChange( const Vector2i& SizeChange ) {
	sendCommonEvent( UIEvent::OnParentSizeChange );
}

Sizei UIControl::getSkinSize( UISkin * Skin, const Uint32& State ) {
	if ( NULL != Skin ) {
		return Skin->getSize( State );
	}

	return Sizei::Zero;
}

Sizei UIControl::getSkinSize() {
	if ( NULL != getSkin() ) {
		return getSkin()->getSize();
	}

	return Sizei::Zero;
}

UIControl * UIControl::getNextWidget() {
	UIControl * Found		= NULL;
	UIControl * ChildLoop	= mChild;

	while( NULL != ChildLoop ) {
		if ( ChildLoop->isVisible() && ChildLoop->isEnabled() ) {
			if ( ChildLoop->isWidget() ) {
				return ChildLoop;
			} else {
				Found = ChildLoop->getNextWidget();

				if ( NULL != Found ) {
					return Found;
				}
			}
		}

		ChildLoop = ChildLoop->mNext;
	}

	if ( NULL != mNext ) {
		if ( mNext->isVisible() && mNext->isEnabled() && mNext->isWidget() ) {
			return mNext;
		} else {
			return mNext->getNextWidget();
		}
	} else {
		ChildLoop = mParentCtrl;

		while ( NULL != ChildLoop ) {
			if ( NULL != ChildLoop->mNext ) {
				if ( ChildLoop->mNext->isVisible() && ChildLoop->mNext->isEnabled() && ChildLoop->mNext->isWidget() ) {
					return ChildLoop->mNext;
				} else {
					return ChildLoop->mNext->getNextWidget();
				}
			}

			ChildLoop = ChildLoop->mParentCtrl;
		}
	}

	return UIManager::instance()->getMainControl();
}

void UIControl::onThemeLoaded() {
}

void UIControl::onChildCountChange() {
}

void UIControl::worldToControl( Vector2i& pos ) const {
	Vector2f Pos( pos.x, pos.y );

	std::list<UIControl*> parents;

	UIControl * ParentLoop = mParentCtrl;

	while ( NULL != ParentLoop ) {
		parents.push_front( ParentLoop );
		ParentLoop = ParentLoop->getParent();
	}

	parents.push_back( const_cast<UIControl*>( reinterpret_cast<const UIControl*>( this ) ) );

	Vector2f scale(1,1);

	for ( std::list<UIControl*>::iterator it = parents.begin(); it != parents.end(); it++ ) {
		UIControl * tParent	= (*it);
		UIControlAnim * tP		= tParent->isAnimated() ? reinterpret_cast<UIControlAnim *> ( tParent ) : NULL;
		Vector2f pPos			( tParent->mRealPos.x * scale.x			, tParent->mRealPos.y * scale.y			);
		Vector2f Center;

		if ( NULL != tP && 1.f != tP->getScale() ) {
			Center = tP->getScaleOriginPoint() * scale;
			scale *= tP->getScale();

			pPos.scale( scale, pPos + Center );
		}

		Pos -= pPos;

		if ( NULL != tP && 0.f != tP->getRotation() ) {
			Center = tP->getRotationOriginPoint() * scale;
			Pos.rotate( -tP->getRotation(), Center );
		}
	}

	Pos = Vector2f( Pos.x / scale.x, Pos.y / scale.y );
	pos = Vector2i( Pos.x / PixelDensity::getPixelDensity(), Pos.y / PixelDensity::getPixelDensity() );
}

void UIControl::controlToWorld( Vector2i& pos ) const {
	Vector2f Pos( (Float)pos.x * PixelDensity::getPixelDensity(), (Float)pos.y * PixelDensity::getPixelDensity() );

	std::list<UIControl*> parents;

	UIControl * ParentLoop = mParentCtrl;

	while ( NULL != ParentLoop ) {
		parents.push_back( ParentLoop );
		ParentLoop = ParentLoop->getParent();
	}

	parents.push_front( const_cast<UIControl*>( reinterpret_cast<const UIControl*>( this ) ) );

	for ( std::list<UIControl*>::iterator it = parents.begin(); it != parents.end(); it++ ) {
		UIControl * tParent	= (*it);
		UIControlAnim * tP		= tParent->isAnimated() ? reinterpret_cast<UIControlAnim *> ( tParent ) : NULL;
		Vector2f pPos			( tParent->mRealPos.x					, tParent->mRealPos.y					);

		Pos += pPos;

		if ( NULL != tP ) {
			Vector2f CenterAngle( pPos.x + tP->mRotationOriginPoint.x, pPos.y + tP->mRotationOriginPoint.y );
			Vector2f CenterScale( pPos.x + tP->mScaleOriginPoint.x, pPos.y + tP->mScaleOriginPoint.y );

			Pos.rotate( tP->getRotation(), CenterAngle );
			Pos.scale( tP->getScale(), CenterScale );
		}
	}

	pos = Vector2i( eeceil( Pos.x ), eeceil( Pos.y ) );
}

UIControl * UIControl::getWindowContainer() {
	UIControl * Ctrl = this;

	while ( Ctrl != NULL ) {
		if ( Ctrl->isType( UI_TYPE_WINDOW ) ) {
			if ( UIManager::instance()->getMainControl() == Ctrl ) {
				return Ctrl;
			} else {
				return static_cast<UIWindow*>( Ctrl )->getContainer();
			}

		}

		Ctrl = Ctrl->getParent();
	}

	return NULL;
}

bool UIControl::isReverseDraw() const {
	return mControlFlags & UI_CTRL_FLAG_REVERSE_DRAW;
}

void UIControl::setReverseDraw( bool reverseDraw ) {
	writeCtrlFlag( UI_CTRL_FLAG_REVERSE_DRAW, reverseDraw ? 1 : 0 );
}

}}
