struct fifo {
  uint8_t head;
  uint8_t tail;
  uint8_t buf[FIFOSIZE];
};

void fifoInit(struct fifo *fifo)
{
  fifo->head=0;
  fifo->tail=0;
}

bool fifoWrite(struct fifo *fifo, uint8_t d)
{
  uint8_t i = (fifo->head + 1) % FIFOSIZE;
  if (i != fifo->tail) {
    fifo->buf[fifo->head] = d;
    fifo->head = i;
    return true;
  } else {
    return false;
  }
}

bool fifoAvail(struct fifo *fifo)
{
  return (fifo->head != fifo->tail);
}

uint8_t fifoRead(struct fifo *fifo)
{
  uint8_t d = 0;
  if (fifoAvail(fifo)) {
    d = fifo->buf[fifo->tail];
    fifo->tail = (fifo->tail + 1) % FIFOSIZE;
  }
  return d;
}

