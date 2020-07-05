#include <string.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include "ScrollbarWindow.h"

ScrollbarWindow::ScrollbarWindow(ScreenBase*& pScreen,
                                 struct MsgPort*& pIdcmpMsgPort,
                                 AMenu* pMenu)
  : WindowBase(pScreen, pIdcmpMsgPort, pMenu),
    m_InnerWindowRight(0),
    m_InnerWindowBottom(0),
    m_SizeImageWidth(18),
    m_SizeImageHeight(10),
    m_pLeftArrowImage(NULL),
    m_pRightArrowImage(NULL),
    m_pUpArrowImage(NULL),
    m_pDownArrowImage(NULL),
    m_pXPropGadget(NULL),
    m_pYPropGadget(NULL),
    m_pLeftArrowButton(NULL),
    m_pRightArrowButton(NULL),
    m_pUpArrowButton(NULL),
    m_pDownArrowButton(NULL)
{

}

ScrollbarWindow::~ScrollbarWindow()
{
  Close();

  if(m_pLeftArrowButton != NULL)
  {
    DisposeObject(m_pLeftArrowButton);
    m_pLeftArrowButton = NULL;
  }

  if(m_pLeftArrowImage != NULL)
  {
    DisposeObject(m_pLeftArrowImage);
    m_pLeftArrowImage = NULL;
  }

  if(m_pRightArrowButton != NULL)
  {
    DisposeObject(m_pRightArrowButton);
    m_pRightArrowButton = NULL;
  }

  if(m_pRightArrowImage != NULL)
  {
    DisposeObject(m_pRightArrowImage);
    m_pRightArrowImage = NULL;
  }

  if(m_pXPropGadget != NULL)
  {
    DisposeObject(m_pXPropGadget);
    m_pXPropGadget = NULL;
  }

  if(m_pDownArrowButton != NULL)
  {
    DisposeObject(m_pDownArrowButton);
    m_pDownArrowButton = NULL;
  }

  if(m_pDownArrowImage != NULL)
  {
    DisposeObject(m_pDownArrowImage);
    m_pDownArrowImage = NULL;
  }

  if(m_pUpArrowButton != NULL)
  {
    DisposeObject(m_pUpArrowButton);
    m_pUpArrowButton = NULL;
  }

  if(m_pUpArrowImage != NULL)
  {
    DisposeObject(m_pUpArrowImage);
    m_pUpArrowImage = NULL;
  }

  if(m_pYPropGadget != NULL)
  {
    DisposeObject(m_pYPropGadget);
    m_pYPropGadget = NULL;
  }
}



bool ScrollbarWindow::Open(InitialPosition initialPos)
{
  if(!WindowBase::Open(initialPos))
  {
    return false;
  }

  return true;
}

void ScrollbarWindow::initialize()
{
  if(m_pScreen == NULL)
  {
    return;
  }

  //
  // Setting up scroll bars and gadgets for the window. They will be
  // attached to the window at opening time
  //
  ULONG imageWidth = 0;   // to successfully store the other images widths
  ULONG imageHeight = 0;  // to successfully store the other images heights

  // Getting the width and height of the current system size gadget
  struct Image* pSizeImage = createImageObj(
    SIZEIMAGE, m_SizeImageWidth, m_SizeImageHeight);

  // the size image is only needed for getting its width and height so
  // it can be disposed right now
  if(pSizeImage != NULL)
  {
    DisposeObject(pSizeImage);
    pSizeImage = NULL;
  }

  // Creating the arrow down image and getting its width and height
  m_pDownArrowImage = createImageObj(DOWNIMAGE, imageWidth, imageHeight);

  // Creating the arrow down gadget
  m_pDownArrowButton = (struct Gadget*) 
                        NewObject(NULL, BUTTONGCLASS,
                        GA_ID, GID_ArrowDown,
                        GA_RelRight, -imageWidth+1,
                        GA_RelBottom, -m_SizeImageHeight-imageHeight+1,
                        GA_Width, imageWidth,
                        GA_Height, imageHeight,
                        GA_DrawInfo, m_pScreen->IntuiDrawInfo(),
                        GA_GZZGadget, TRUE,
                        GA_RightBorder, TRUE,
                        GA_Image, m_pDownArrowImage,
                        ICA_TARGET, ICTARGET_IDCMP,
                        TAG_DONE, NULL);

  // Creating the arrow up image and getting its width and height
  m_pUpArrowImage = createImageObj(UPIMAGE, imageWidth, imageHeight);

  // Creating the arrow down gadget
  m_pUpArrowButton = (struct Gadget*) 
    NewObject(NULL, BUTTONGCLASS,
              GA_Previous, m_pDownArrowButton,
              GA_ID, GID_ArrowUp,
              GA_RelRight, -imageWidth+1,
              GA_RelBottom, -m_SizeImageHeight-imageHeight-imageHeight+1,
              GA_Width, imageWidth,
              GA_Height, imageHeight,
              GA_DrawInfo, m_pScreen->IntuiDrawInfo(),
              GA_GZZGadget, TRUE,
              GA_RightBorder, TRUE,
              GA_Image, m_pUpArrowImage,
              ICA_TARGET, ICTARGET_IDCMP,
              TAG_DONE, NULL);

  // Creating the vertical proportional gadget / slider
	m_pYPropGadget = (struct Gadget*) 
    NewObject(NULL, PROPGCLASS,
              GA_Previous, m_pUpArrowButton,
              GA_ID, GID_PropY,
              GA_RelRight, -m_SizeImageWidth+4,
              GA_Top, m_pScreen->BarHeight(),
              GA_Width, m_SizeImageWidth-6,
              GA_RelHeight, -m_SizeImageHeight-imageHeight-imageHeight-m_pScreen->BarHeight()-1,
              GA_DrawInfo, m_pScreen->IntuiDrawInfo(),
              GA_GZZGadget, TRUE,
              GA_RightBorder, TRUE,
              PGA_Freedom, FREEVERT,
              PGA_Borderless, TRUE,
              PGA_NewLook, TRUE,
              PGA_Total, 100,
              PGA_Top, 0, // TODO remove??
              PGA_Visible, 100,
              ICA_TARGET, ICTARGET_IDCMP,
              TAG_DONE, NULL);

  // Creating the arrow left image and getting its width and height
  m_pRightArrowImage = createImageObj(RIGHTIMAGE, 
                                      imageWidth, 
                                      imageHeight);

  // Creating the arrow right gadget
  m_pRightArrowButton = (struct Gadget*) 
    NewObject(NULL, BUTTONGCLASS,
              GA_Previous, m_pYPropGadget,
              GA_ID, GID_ArrowRight,
              GA_RelRight, -m_SizeImageWidth-imageWidth+1,
              GA_RelBottom, -imageHeight+1,
              GA_Width, imageWidth,
              GA_Height, imageHeight,
              GA_DrawInfo, m_pScreen->IntuiDrawInfo(),
              GA_GZZGadget, TRUE,
              GA_BottomBorder, TRUE,
              GA_Image, m_pRightArrowImage,
              ICA_TARGET, ICTARGET_IDCMP,
              TAG_DONE, NULL);

  // Creating the arrow left image and getting its width and height
  m_pLeftArrowImage = createImageObj(LEFTIMAGE, imageWidth, imageHeight);

  // Creating the arrow left gadget
  m_pLeftArrowButton = (struct Gadget*) 
    NewObject(NULL, BUTTONGCLASS,
              GA_Previous, m_pRightArrowButton,
              GA_ID, GID_ArrowLeft,
              GA_RelRight, -m_SizeImageWidth-imageWidth-imageWidth+1,
              GA_RelBottom, -imageHeight+1,
              GA_Width, imageWidth,
              GA_Height, imageHeight,
              GA_DrawInfo, m_pScreen->IntuiDrawInfo(),
              GA_GZZGadget, TRUE,
              GA_BottomBorder, TRUE,
              GA_Image, m_pLeftArrowImage,
              ICA_TARGET, ICTARGET_IDCMP,
              TAG_DONE, NULL);

  // Creating the horizontal proportional gadget / slider
  m_pXPropGadget = (struct Gadget*) 
    NewObject(NULL, PROPGCLASS,
              GA_Previous, m_pLeftArrowButton,
              GA_ID, GID_PropX,
              GA_Left, m_pScreen->IntuiScreen()->WBorLeft,
              GA_RelBottom, -m_SizeImageHeight+3,
              GA_RelWidth, -m_SizeImageWidth-imageWidth-imageWidth-m_pScreen->IntuiScreen()->WBorLeft-1,
              GA_Height, m_SizeImageHeight-4,
              GA_DrawInfo, m_pScreen->IntuiDrawInfo(),
              GA_GZZGadget, TRUE,
              GA_BottomBorder, TRUE,
              PGA_Freedom, FREEHORIZ,
              PGA_Borderless, TRUE,
              PGA_NewLook, TRUE,
              PGA_Total, 100,
              //PGA_Left, 0,  // TODO remove??
              PGA_Visible, 100,
              ICA_TARGET, ICTARGET_IDCMP,
              TAG_DONE, NULL);

  // Set the default title
  SetTitle("ScrollbarWindow");

  // Setting the IDCMP messages we want to receive for this window
  setIDCMP(IDCMP_VANILLAKEY |     // Inform about normal key presses
           IDCMP_RAWKEY |         // Inform about RAW key presses
           IDCMP_IDCMPUPDATE);    // Inform about BOOPSI gadget updates

  // Setting the first gadget of the gadet list for the window
  setFirstGadget(m_pDownArrowButton);

  m_bInitialized = true;

}

void ScrollbarWindow::HandleIdcmp(ULONG msgClass, UWORD msgCode, APTR pItemAddress)
{
  if(!IsOpen())
  {
    return;
  }

  switch (msgClass)
  {
    case IDCMP_IDCMPUPDATE:
    {
      ULONG tagData = GetTagData(GA_ID, 0, (struct TagItem *)pItemAddress);
      switch(tagData)
      {
        case ScrollbarWindow::GID_PropX:
        {
          // Get the top value of the prop gadget from the tag data of
          // the message
          size_t newX = GetTagData(PGA_Top, 0, (struct TagItem *)
            pItemAddress);

          // If there are other GID_PropX addressed to this window,
          // delete them and get the top value from the latest found
          // message.
          extractLatestPropGadTopValue(GID_PropX, newX);

          // Pass the latest found newX value to the childs which have
          // to implement this abstract method
          XChangedHandler(newX);

          break;
        }

        case ScrollbarWindow::GID_PropY:
        {
          // Get the top value of the prop gadget from the tag data of
          // the message
          size_t newY = GetTagData(PGA_Top,
                                   0,
                                   (struct TagItem *) pItemAddress);

          // If there are other GID_PropY addressed to this window,
          // delete them and get the top value from the latest found
          // message.
          extractLatestPropGadTopValue(GID_PropY, newY);

          // Pass the latest found newY value to the childs which have
          // to implement this abstract method
          YChangedHandler(newY);

          break;
        }

        case ScrollbarWindow::GID_ArrowLeft:
        {
          XDecrease(2);
          break;
        }

        case ScrollbarWindow::GID_ArrowRight:
        {
          XIncrease(2);
          break;
        }

        case ScrollbarWindow::GID_ArrowUp:
        {
          YDecrease(2);
          break;
        }

        case ScrollbarWindow::GID_ArrowDown:
        {
          YIncrease(2);
          break;
        }

      }
      break;
    }

    case IDCMP_RAWKEY:
    {
      if(msgCode == CURSORLEFT)
      {
        XDecrease(2);
      }
      else if(msgCode == CURSORRIGHT)
      {
        XIncrease(2);
      }
      else if(msgCode == CURSORDOWN)
      {
        YIncrease(2);
      }
      else if(msgCode == CURSORUP)
      {
        YDecrease(2);
      }
      break;
    }
  }
}


void ScrollbarWindow::calcSizes()
{
  if(m_pScreen == NULL)
  {
    return;
  }

  // (Re-)calculate some values that may have be changed by re-sizing
  m_InnerWindowRight = m_pWindow->Width
    - m_pScreen->IntuiScreen()->WBorLeft
    - m_SizeImageWidth;

  m_InnerWindowBottom = m_pWindow->Height
    - m_pScreen->BarHeight()
    - m_SizeImageHeight;
}


void ScrollbarWindow::setXScrollLeft(int left)
{
  if(m_pXPropGadget == NULL)
  {
    return;
  }

  if(left < 0)
  {
    return;
  }

  SetGadgetAttrs(m_pXPropGadget, m_pWindow, NULL,
                 PGA_Top, left,
                 TAG_DONE);
}


void ScrollbarWindow::setYScrollTop(int top)
{
  if(m_pYPropGadget == NULL)
  {
    return;
  }

  if(top < 0)
  {
    return;
  }

  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
                 PGA_Top, top,
                 TAG_DONE);
}


void ScrollbarWindow::setXScrollPotSize(int maxVisibleChars,
                                        int totalChars)
{
  if(m_pXPropGadget == NULL)
  {
    return;
  }

  if(totalChars < 0)
  {
    totalChars = maxVisibleChars;
  }

  // Set the size of x-scrollbar "potentiometer"
  SetGadgetAttrs(m_pXPropGadget,
                 m_pWindow,
                 NULL,
                 PGA_Total, totalChars,
                 PGA_Visible, maxVisibleChars,
                 TAG_DONE);

}

void ScrollbarWindow::setYScrollPotSize(int maxVisibleLines,
                                        int totalLines)
{
  if(m_pYPropGadget == NULL)
  {
    return;
  }

  if(totalLines < 0)
  {
    totalLines = maxVisibleLines;
  }

  // Set the size of y-scrollbar "potentiometer"
  SetGadgetAttrs(m_pYPropGadget,
                 m_pWindow,
                 NULL,
                 PGA_Total, totalLines,
                 PGA_Visible, maxVisibleLines,
                 TAG_DONE);

}


void ScrollbarWindow::extractLatestPropGadTopValue(GadgetId gadgetId,
                                                   size_t& lastVal)
{
  Forbid();

  struct MsgPort* pIdcmpMsgPort = m_pWindow->UserPort;

  struct IntuiMessage* pMessage;
  pMessage = (struct IntuiMessage*) pIdcmpMsgPort->mp_MsgList.lh_Head;

  struct Node* pSuccessor;

  while((pSuccessor = pMessage->ExecMessage.mn_Node.ln_Succ) != NULL)
  {
    if(pMessage->IDCMPWindow == m_pWindow &&
        pMessage->Class == IDCMP_IDCMPUPDATE)
    {
      ULONG msgTagData = GetTagData(GA_ID, 0,
        (struct TagItem *)pMessage->IAddress);

      if(msgTagData == gadgetId)
      {
        lastVal = GetTagData(PGA_Top, 0, (struct TagItem*)
          pMessage->IAddress);

        // Intuition is about to free this message. Make sure
        // that we have politely sent it back.
        Remove((struct Node*) pMessage);
        ReplyMsg((struct Message*) pMessage);
      }
    }

    pMessage = (struct IntuiMessage*) pSuccessor;
  }

  // Enable multi tasking
  Permit();
}

