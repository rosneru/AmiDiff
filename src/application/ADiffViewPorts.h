#ifndef ADIFFVIEW_PORTS_H
#define ADIFFVIEW_PORTS_H

#include <exec/ports.h>

class ADiffViewPorts
{
public:
  ADiffViewPorts();
  virtual ~ADiffViewPorts();

  struct MsgPort* Idcmp() const;
  struct MsgPort* Progress() const;
  struct MsgPort* Workbench() const;

private:
  struct MsgPort* m_pMsgPortIDCMP;
  struct MsgPort* m_pMsgPortProgress;
  struct MsgPort* m_pMsgPortWorkbench;
};

#endif
