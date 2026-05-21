import axios from 'axios'
import { ElMessage } from 'element-plus'
import 'element-plus/theme-chalk/el-message.css'
import { useUserStore } from '@/stores/userStore'
import router from '@/router'

const httpInstance = axios.create({
    baseURL: 'http://localhost:8000',
    timeout: 5000
})

httpInstance.interceptors.request.use(config => {
    return config
}, e => Promise.reject(e))

httpInstance.interceptors.response.use(res => res.data, e => {
    const status = e.response?.status
    const url = e.config?.url || 'unknown api'

    console.error('HTTP request failed:', {
        url,
        status,
        message: e.response?.data?.message || e.message,
        response: e.response?.data
    })

    ElMessage({
        type: 'warning',
        message: status
            ? `${url} request failed: ${status}`
            : `${url} request failed. Please check whether the backend is running.`
    })

    if (status === 401) {
        const userStore = useUserStore()
        userStore.clearUserInfo()
        router.push('/login')
    }

    return Promise.reject(e)
})

export default httpInstance
