FROM debian:buster as builder

RUN apt-get update && \
    apt-get install -y \
      git \
      g++ \
      make \
      cmake \
      libboost-dev \
      libsndfile-dev \
      libyaml-cpp-dev \
      scons

RUN git clone --recursive https://github.com/ebu/libear.git && \
    cd libear/ && \
    mkdir build && \
    cd build && \
    cmake .. -DBUILD_SHARED_LIBS=TRUE && \
    make && \
    make install

RUN git clone https://github.com/IRT-Open-Source/libadm.git && \
    cd libadm && \
    git checkout 0.11.0 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

RUN git clone https://github.com/IRT-Open-Source/libbw64.git && \
    cd libbw64 && \
    git checkout 0.10.0 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

RUN git clone https://github.com/media-cloud-ai/adm_engine.git && \
	cd adm_engine && \
	git checkout v1.0.0 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

ADD . /loudness_validator

WORKDIR /loudness_validator

RUN scons --adm-loudness-worker

FROM mediacloudai/c_amqp_worker:v0.3.0

COPY --from=builder /usr/local/lib/ /app/loudness_validator/lib/
COPY --from=builder /loudness_validator/build/release/app/bin/adm-loudness-analyser /app/loudness_validator/bin/adm-loudness-analyser
COPY --from=builder /loudness_validator/build/release/src/admLoudnessAnalyser/lib/ /app/loudness_validator/lib/
COPY --from=builder /loudness_validator/build/release/worker/lib/ /app/loudness_validator/worker/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libyaml-cpp.so* /app/loudness_validator/lib/
COPY --from=builder /lib/x86_64-linux-gnu/libm.so* /app/loudness_validator/lib/

WORKDIR /app/loudness_validator

ENV AMQP_QUEUE job_adm_loudness
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:/app/loudness_validator/lib
ENV WORKER_LIBRARY_FILE /app/loudness_validator/worker/libadmloudnessworker.so
ENV PATH $PATH:/app/loudness_validator/bin

CMD c_amqp_worker
