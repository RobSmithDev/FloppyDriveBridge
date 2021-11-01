TARGET  = FloppyBridge.so

SOURCE  = floppybridge/ArduinoFloppyBridge.cpp \
          floppybridge/ArduinoInterface.cpp \
          floppybridge/CommonBridgeTemplate.cpp \
          floppybridge/ftdi.cpp \
          floppybridge/GreaseWeazleBridge.cpp \
          floppybridge/GreaseWeazleInterface.cpp \
          floppybridge/RotationExtractor.cpp \
          floppybridge/SerialIO.cpp \
          floppybridge/SuperCardProBridge.cpp \
          floppybridge/SuperCardProInterface.cpp
DEPS	= $(SOURCE:%.cpp=%.d)

CPPFLAGS +=-shared -fPIC -ldl

.PHONY : all clean

$(TARGET) : $(SOURCE)
	g++ $(CPPFLAGS) -o $(TARGET) $(SOURCE)

all : $(TARGET)

install : $(TARGET)
	cp $(TARGET) /usr/lib

clean :
	rm -f $(DEPS)
	rm -f $(TARGET)

cleanprofile:
	rm -f $(OBJS:%.o=%.gcda)

-include $(DEPS)
