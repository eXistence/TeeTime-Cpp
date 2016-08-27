/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <teetime/stages/AbstractConsumerStage.h>

namespace teetime
{ 
  template<typename T>
  class DistributorStage final : public AbstractConsumerStage<T>
  {
  public:
    DistributorStage(const char* debugName = "DistributorStage")
    : AbstractConsumerStage<T>(debugName)
    , m_next(0)
    {}

    OutputPort<T>& getNewOutputPort() 
    {
      OutputPort<T>* p = AbstractStage::addNewOutputPort<T>();

      return *p;
    }

  private:
    virtual void execute(T&& value) override
    {
      const uint32 numOutputPorts = AbstractStage::numOutputPorts();
      if(numOutputPorts == 0) 
      {
        throw std::logic_error("distributor stage needs at least on output port");
      }

      while (true)
      {
        uint32 portIndex = m_next;
        m_next = portIndex + 1;
        if (m_next == numOutputPorts)
        {
          m_next = 0;
        }

        //TEETIME_TRACE() << "distributing value '" << value << "' to port " << m_next;
        auto abstractPort = AbstractStage::getOutputPort(portIndex);
        assert(abstractPort);

        auto typedPort = unsafe_dynamic_cast<OutputPort<T>>(abstractPort);
        assert(typedPort);

        if (typedPort->trySend(std::move(value)))
        {
          break;
        }
      }
    }    

    uint32 m_next;
  };
}