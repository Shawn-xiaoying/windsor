<p align='center'>
  <img src='screenshots/OnebaseText.svg' alt=''  width='200' />
</p>

## Introduce

Welcome to OneBase's GitHub Repository!

Founded by an Engineer, a Hyperbaric Oxygen Specialist, and a Scandinavian Creative, OneBase is dedicated to creating the easiest and most efficient hyperbaric oxygen therapy experience.

We design and build high-quality chambers using the best materials and technology, ensuring safety, durability, and aesthetic appeal. Our commitment extends beyond products to providing education and support, empowering individuals to take control of their health journey.

Through innovative, result-driven solutions, we strive to help our clients achieve their wellness goals with confidence and ease. Join us in revolutionizing hyperbaric oxygen therapy.

<p>  
  <img src="screenshots/me_health_score.png" width="24%" />
  <img src="screenshots/devices.jpeg" width="24%" />
  <img src="screenshots/control.png" width="24%" />
</p>

## Table of contents

-   [Overall](#overall)
-   [Project Structure](#project-structure)
    -   [File Structure](#file-structure) (_Structure project's files and folders_)
-   [Project setup](#project-setup)
    -   [Prerequisites](#prerequisites)
    -   [Database](#database) (_Instructions setup database_)
    -   [Installation](#installation) (_The guideline install package and generate schema_)
    -   [API Setup](#api-setup)
    -   [Upload Setup](#upload-setup) (_Service used to upload files to aws s3_)
    -   [Media Setup](#media-setup)
    -   [Admin Portal Setup](#admin-portal-setup) (_Service for user with role Admin_)
    -   [Mobile App Setup](#mobile-app-setup) (_Instructions setup, build and deploy React Native app_)
        -   [Versioning](#Versioning)
        -   [Android](#android)
        -   [IOS](#ios)
    -   [Mobile App - Tablet Control Setup](src/apps/general/mobile/TabletControl/README.md) (_Instructions setup, build and deploy React Native app_)
-   [Git Follow](#git-follow) (_Git follow that using for Onebase_)
    -   [Branch Management](#branch-management)
    -   [Pull Request](#pull-request)
-   [Third Party](#third-party) (_List third party using in project_)

## Overall

The system includes main components such as API service, Upload service, Media Service, Static Service, Admin Portal and Mobile App:

-   API Service => main api of OneBase and built on NodeJS(Express)
-   Upload Service => service only for upload to S3 and built on NodeJS(Express)
-   Media Service => handle media of project
-   Static Service => built on Caddyfile
-   Admin Portal => admin portal for admin and built on ReactJS
-   Mobile App => mobile app for user and built on React Native

## Project Structure

The entire Onebase platform is built from a `Monorepo` and the file structure have been formed around it. The reasoning around this is that we want to give us a sustainable solution for different products and at the same time give us the following:

-   Share utils, components and content between
    1.  all projects
    2.  all projects on the same platform (server/native/web, frontend/backend)
    3.  all projects on a base level (admin/app/etc..)
    4.  all projects with the Onebase graphical profile (main/app/diner/etc...)
    5.  same projects but different platforms (web/native app, etc)

### File Structure

-   projects: /src
    -   apps
        -   brands (React Native build to app)
        -   general
            -   browser
                -   admin (ReactJS built in Admin portal)
                -   main (NextJS server side combine to landing page)
            -   server
                -   db (Docker compose build image contain DB postgreSQL)
                -   upload (service only for upload to S3 and built on NodeJS(Express))
                -   media (handle asset to file on S3)
                -   api (main api of OneBase and built on NodeJS(Express) + tRPC)
            -   mobile
                -   BLELibDemo (React Native app to demo the usage of BLE library with native layer)
                -   TabletControl (React Native template)
        -   templates/native/main (React Native template before synchronize to brands)
    -   shared
        -   app (Tools for the native environment)
            -   styles (Native specific styles)
            -   ui (Native specific UI/components)
            -   utils (Native specific utils)
        -   common (Tools for all platforms and projects)
            -   ble (all function and constant control BLE)
            -   animation
            -   client (utils etc specific for frontend - both web and app)
            -   config (Config used throughout all projects)
            -   data (Common pure data throughout all projects)
            -   errors (Errors of all projects)
            -   logging (Shared logging module - integrates with Sentry)
            -   static (Shared Static data such as images etc)
            -   stories (Shared stories - we might want to move this to content?)
            -   styled (Onebase's wrapper around styled-components)
            -   styles (Shared styles)
            -   types (Shared types)
            -   ui (Shared UI/components)
            -   utils (Platform agnostic utils)
        -   content (Contains shared content & components specific to one project but shared between both native and web)
            -   all (Specific for any project - unclear what this is but maybe user terms? Does it compete with @onebase/data?)
            -   app (Specific for the Onebase App)
        -   web (Tools for the web environment)
            -   styles (Web specific styles)
            -   ui (Web specific UI/components)
    -   tools
        -   babel (Shared babel config)
        -   eslint-config (Shared eslint config)
        -   webpack (Shared webpack config)

## Project setup

FYI, This project works fine on Linux and Mac.

### Prerequisites

-   Required
    -   NodeJS ([https://nodejs.org/en/download/](https://nodejs.org/en/download/)) (the platform was developed with @18, but should work at least @20 as well)
    -   Yarn.
    -   XCode.
    -   Android Studio.
    -   PostgreSQL.
-   Recommended
    -   Docker ([https://docs.docker.com/desktop/mac/install/](https://docs.docker.com/desktop/mac/install/w)) (Optional)
    -   VS Code ([https://code.visualstudio.com/download](https://code.visualstudio.com/download))

### Repository

1.  Make sure you have gotten access to the Github organization
2.  Download the repo

### Database

You have two top options:

**Docker setup**

1. Make sure `docker` is installed. I had a global postgres instance on my local computer for all my projects. You can also use the `yarn db start` command to spin up a postgres container with the `postgres:postgres` as credentials. Check `src/apps/general/server/db/docker-compose.yml` file for more details.
2. Create the database `onebase` in your postgres instance.

**Manually setup**

1. Install postgres locally on your computer set it up.
2. Create the database `onebase` in your postgres instance.

### Installation

The following instructions are general for the entire project.

1. Open the project in your terminal.
2. Install all dependencies by running `yarn`. This command will be completed with errors. You don't have to worry about the errors right now.
3. Initialize the prisma types by running the `yarn db:gen` command.
4. Complete the dependency installation by running `yarn` again. Now it should complete without errors. There might be some warnings, but these do not matter.
5. Now you are all setup code-wise. However, depending on what platform you have to run, there might be a couple of extra steps you need to perform. Read more below

```
The reason for why we need to run the `yarn` command twice is due to some ReactNative inconveniences, but it works for now.
```

### API Setup

Currently, Api using tRPC server combined concepts from REST and GraphQL. Follow document tRPC here: https://trpc.io/docs/quickstart.

Make sure to have the [Database](#Database) is setup.

1. Run `yarn api start` to try and start the api.
    1. Got some errors? Good! That means the environment variables runtime typing system works as it should and this is a good time to show you where you can put your environment variables.
2. Create the `src/apps/general/server/api/env/env.secrets.json` file from `src/apps/general/server/api/env/env.default.json`.

    1. We'll go more into detail of this in time, but for now it is sufficient for you to know that you can here you can insert secret values. You can also insert them namespaced depending on what environment it is running (prod, stage, local).
    2. Put the following content into the `env.secrets.json` file:

    ```json
    {
    	"default": {
    		"DEVICE": "SERVER",
    		"PLATFORM": "API",
    		"SENTRY_PROJECT_SLUG": "...",
    		"SENTRY_DSN": "...",
    		"ONESIGNAL__USER_AUTH_KEY": "...",
    		"ONESIGNAL__API_KEY": "...",
    		"ONESIGNAL__APP_ID": "...",
    		"SENDGRID__API_KEY": "...",
    		"SENDGRID__FROM": "...",
    		"DB_URL": "...",
    		"SALT__STATIC": "...",
    		"JWT__SECRET": "...",
    		"JWT__ACCESS_TOKEN__EXPIRATION": "90d",
    		"JWT__REFRESH_TOKEN__EXPIRATION": "90d",
    		"AWS__ACCESS_KEY_ID": "...",
    		"AWS__SECRET_ACCESS_KEY": "...",
    		"STRIPE__API_KEY": "...",
    		"STRIPE__CURRENCY": "...",
    		"STRIPE__WEBHOOK": "...",
    		"STRIPE__WEBHOOK_SECRET": "",
    		"POSTHOG__API_KEY": "...",
    		"POSTHOG__PROJECT_ID": "..."
    	}
    }
    ```

3. Run `yarn db:prisma:reset` to generate the prisma types, push the latest schema to the database and run the seed.

    1. If successful, you should now have the dev database populated with some data, and ready for starting the development server.

4. Now try to run `yarn api start` again.

5. If you want to create table or change anything in database. Let's check follow:

    1. If you want to create new table find to `src/shared/db/db.source/models` and create new folder with name same as table name. And then create file `index.ts`.
    2. if you want to change anything find to folder same name table and edit file `index.ts`.
    3. Run command `yarn db:dev` to sync model and schema for prisma.
    4. Run command `yarn db:prisma:save {name}` to make file migration.
    5. Run command `yarn db:prisma:deploy` to change database.

6. Why is tRPC? How to create a tRPC API? -> follow this [document](/docs/platform/server.md)

```
Note: If you want to install any dependency for API, you can run command below:
yarn api add {dependency_name}
```

### Upload Setup

1. If you have run all the steps above, running the admin panel should be as easy as running `yarn upload start`.

```
Note: If you want to install any dependency for Upload, you can run command below:
yarn upload add {dependency_name}
```

### Media Setup

1. If you have run all the steps above, running the admin panel should be as easy as running `yarn media start`.

```
Note: If you want to install any dependency for Media, you can run command below:
yarn media add {dependency_name}
```

### Admin Portal Setup

1. If you have run all the steps above, running the admin panel should be as easy as running `yarn admin start`.
2. Go to `http://localhost:3002` and you should see the admin panel.
3. Login with `superadmin@onebase.health` and `testing`

```
Note: If you want to install any dependency for Admin Portal, you can run command below:
yarn admin add {dependency_name}
```

### Mobile App Setup

The Mobile App is built as a white label platform. Therefore we have the concepts of `brands` which you can find in the `src/apps/brands` folder. You should at least have the onebase brand in your repository. Let's try to make it spin!

Go to the folder `src/apps/brands/onebase`

1. Create an empty placeholder file at `.config/overrides/native/main/android/app/release.keystore`.
    1. This will in the future be a secret file to deploy your android app, but it is not used in development - hence it can be empty right now.
2. Create an empty (`{}`) `.config/overrides/native/main/env/env.secrets.json` file
3. Run `onebase brand sync --alias onebase`.
    1. There might be some additional softwares that you need to install here (e.g. gem, cocoapods, etc.). The error messages is not always the best, so if you get an error, try to google your way to until this works.
    2. Once started, this command will take some time to finish. But once it is finished, you are very close to finishing this tutorial.
4. For IOS: Run `yarn napp:onebase run:ios:pure` to install the app on the simulator. This will also run the app, but without metro. So close the app once it has started.
5. For Android: Run `yarn napp:onebase run:android:pure` to install the app on the simulator or real device.
6. Run `yarn napp:onebase start` to start the metro bundler.
7. Now open the app from the simulator start screen and wait for it to load.
8. Login with `user@onebase.health` and `testing`.
9. Now you should be in the onboarding. Just click through everything (the payment screen should be mocked, so you'll get through that as well) until you are at the start screen of the app.
10. That's it! You have now successfully finished the Onebase tutorial and can start developing!

### Versioning

The versioning is important in both the Android and Apple world. So to make this process a easier we use the package `react-native-version` that syncs the `package.json` file with the react-native files. Since we have defined this package as a `postversion` hook, all that is needed is to run `yarn version`. However, this, in our project is handled globally by lerna in the root folder as described below.

### Android

Google Play have 4 (optional) "tracks" that are used for incremental deployment: `internal`, `alpha`, `beta` and `production`. We also have two options of building the project:

-   `apk`: Android Application Package. A sort of a runnable image that can be directly installed on an android device. You can build apk file by command `yarn apk:build`.
-   `aab`: Android App Bundle. "The new way" of deploying android apps which decreases the app size by skipping a lot of generic data that is needed for running it separately. This is not directly installable on an android device but have to be deployed by Google Play.

In order to deploy to any of these the project has to be built with a keystore that contains some cryptographic data that signs the app to our account. There are two keystore files:

-   `debug.keystore`: Used in development. This file is only used locally and can (should?) be generated for each new user.
-   `release.keystore`: Used in apk/aab that is up for deployment. This file is a secret and is synced (uploaded to) with Google Play in order to ensure that our new build is made by the same developer.

### IOS

IOS is a little bit trickier than Android and their certificate/signing process is bit moremore complicated. They have something called Provisioning Profiles and their corresponding Certificates.

> Sign with Apple

-   Open your project in `Xcode`, select `Targets` project and click tab ` Signing & Capabilities`.
-   Click `+ Capability` to the left of the build configurations, search and select `Sign in with Apple` in list capabilities.

> Build and deploy

-   Select `Targets` project and `OneSignalNotificationServiceExtension`, click tab ` Signing & Capabilities`.
-   In `Signing`, select the account that has the project setup.
-   Choose `Product` → `Archive` to distribute app.
-   After archive, to push the app to `TestFlight`, click `Distribute app`, select the `TestFlight & App Strore` option then press the `Distribute` button.

```
Note: If you want to install any dependency for API, you can run command below:
yarn workspace @onebase/template.native.main add {dependency_name}
```

## Git Follow

### Branch Management

Regarding branching and branch management on OneBase, we are currently dividing into main branches such as `main`, `staging`, `dev`, `sprint`, `feature`. The `main`, `staging` branch will be protected. We can comment directly into these branchs and only dev lead can merge PR on it.

-   The `main` branch will be deployed to the production environment
-   The `staging` branch will be deployed to the staging environment
-   The `dev` branch will be deployed to the development environment

A sprint branch will be created when each new sprint start, example: `sprint/sprint-12` checkout from `dev` branch.

The feature branch is divided into 2 main branches:

-   `feature/WAP` is used to do tasks,
-   `feature/SW` is used to fix bugs.

The branch name will be the task id on Asana. Once completed, we will create a new PR to sprint branch. We will review and merge PR on it.

After sprint completed, we will create PR to `dev` branch and merge it.

### Pull Request

-   Format of PR title: [Asana-Task-ID] Ansana-Task-Description. For example `[WAP-9-230] Update README file`
-   The PR must be added the labels, milestone and assigned to reviewer(dev lead). Dev lead can review and comment into PR if have any issue and team member can update it.

## Third party

-   `Sentry` (Capture error on web and mobile app)
-   `Sendgrid` (Service used to send email)
-   `Posthog` (User Tracking service)
-   `Stripe` (Service payment)
-   `Onesignal` (Service push notification to mobile app)
-   `ImageKit` (Image transformation)
-   `S3` (A Service of AWS to storage files)
    and wearable data)
