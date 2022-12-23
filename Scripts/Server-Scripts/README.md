# Server Deployment and Running
FinalCypher servers are dockerized and uploaded to Google Cloud Container Registry and is then fetched using Google Game Servers using config files that point to the latest server containerized image. After pushing the new server image to the google container registry the new image is assigned to google game servers to be used in the upcoming deployments.

# A. [Build Server Docker Image] Steps to create server image and push it to container registry
1. Run command in root with tag name (if not supplied, latest is used, but recommended to version). `docker build -f .\DockerFile.server -t fc-server:[tag] .` f.e `docker build -f .\DockerFile.server -t fc-server:0.2.5 .`
2. Inspect Images with `docker images`
3. Create target image with source image using `docker tag fc-server:[tag] gcr.io/finalcypher-a9ce7/fc-server:[tag]` to point to gcr. f.e `docker tag fc-server:0.2.6 gcr.io/finalcypher-a9ce7/fc-server:0.2.6`
4. Push image to the container registry using `docker push gcr.io/finalcypher-a9ce7/fc-server:[tag]`

# B. [Deploy Image to GCloud game servers]
1. Go to `https://console.cloud.google.com/game/gameserverconfigs` and authenticate
2. Go to `fc-deployment` and create a new config with the format `fc-config-[t-a-g]`
3. Modify the current working fleet config [Make sure to clear the default fleet config]
4. Add the new image registry to the fleet `template.template.spec.containers.imgage` with the format `fc-fleet-[t.a.g]`
5. Manage Rollout. Map the new config as a default config or add it in the override to fc-realm. 

# Gcloud auth for gcr
1. `gcloud auth login`
2. `gcloud auth configure-docker`