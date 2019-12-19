
# ADM Engine AMQP worker

The purpose of this worker is to integrate the __ADM Loudness Analyser__ tool easily into a message broker environment.
It is based on the [c_amqp_worker](https://github.com/media-cloud-ai/c_amqp_worker) library.
The worker can handle AMQP message under JSON format. Here are some usage examples:

 * Analyse BW64/ADM file loudness:
    ```json
    {
      "job_id": 123,
      "parameters": [
        {
          "id": "input",
          "type": "string",
          "value": "/path/to/bw64_adm.wav"
        }
      ]
    }
    ```


 * Analyse BW64/ADM file loudness, display result details and write updated ADM with the computed loudness values into an output file:
    ```json
    {
      "job_id": 123,
      "parameters": [
        {
          "id": "input",
          "type": "string",
          "value": "/path/to/bw64_adm.wav"
        },
        {
          "id": "output",
          "type": "string",
          "value": "/path/to/output/bw64_adm.wav"
        },
        {
          "id": "display",
          "type": "boolean",
          "value": true
        }
      ]
    }
    ```


 * Analyse specified ADM element loudness and write updated ADM with the computed loudness values into an output file:
    ```json
    {
      "job_id": 123,
      "parameters": [
        {
          "id": "input",
          "type": "string",
          "value": "/path/to/bw64_adm.wav"
        },
        {
          "id": "output",
          "type": "string",
          "value": "/path/to/output/bw64_adm.wav"
        },
        {
          "id": "element_id",
          "type": "string",
          "value": "APR_1002"
        }
      ]
    }
    ```


 * Analyse specified ADM element loudness, display result details, apply gain correction if necessary, and write updated ADM with the computed loudness values into an output file:
    ```json
    {
      "job_id": 123,
      "parameters": [
        {
          "id": "input",
          "type": "string",
          "value": "/path/to/bw64_adm.wav"
        },
        {
          "id": "output",
          "type": "string",
          "value": "/path/to/output/bw64_adm.wav"
        },
        {
          "id": "element_id",
          "type": "string",
          "value": "APR_1002"
        },
        {
          "id": "display",
          "type": "boolean",
          "value": true
        },
        {
          "id": "correction",
          "type": "boolean",
          "value": true
        }
      ]
    }
    ```


 * Analyse specified ADM element loudness, display result details, apply gain correction if necessary with peak limiter, and write updated ADM with the computed loudness values into an output file:
    ```json
    {
      "job_id": 123,
      "parameters": [
        {
          "id": "input",
          "type": "string",
          "value": "/home/valentin/Musique/documentaire/documentaire-extract.wav"
        },
        {
          "id": "output",
          "type": "string",
          "value": "/home/valentin/Musique/documentaire/worker"
        },
        {
          "id": "element_id",
          "type": "string",
          "value": "APR_1002"
        },
        {
          "id": "display",
          "type": "boolean",
          "value": true
        },
        {
          "id": "correction",
          "type": "boolean",
          "value": true
        },
        {
          "id": "limiter",
          "type": "boolean",
          "value": true
        }
      ]
    }
    ```

