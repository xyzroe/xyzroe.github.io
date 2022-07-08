module.exports = {
  apps : [{
    name: 'Bongard 12.64',
    script: 'server.py',
// pm2 start main.py --interpreter python3
    // Options reference: https://pm2.keymetrics.io/docs/usage/application-declaration/
   // args: '
    interpreter: 'python3',
    instances: 1,
    autorestart: true,
    watch: true,
    max_memory_restart: '1G',
    env: {
      NODE_ENV: 'development'
    },
    env_production: {
      NODE_ENV: 'production'
    }
  }],

};
