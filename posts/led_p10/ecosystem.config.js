module.exports = {
  apps : [{
    name: 'LED_P10_txt',
    script: 'led_p10.py',
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
