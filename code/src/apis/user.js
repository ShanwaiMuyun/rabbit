import httpInstance from '@/utils/http'

export const loginAPI = ({ account, password }) => {
    return httpInstance({
        url: '/login',
        method: 'POST',
        data: {
            username: account,
            password
        }
    })
}

export const registerAPI = ({ account, password, role = 'buyer' }) => {
    return httpInstance({
        url: '/register',
        method: 'POST',
        data: {
            username: account,
            password,
            role
        }
    })
}

export const addMessageAPI = ({ username, content }) => {
    return httpInstance({
        url: '/messages',
        method: 'POST',
        data: {
            username,
            content
        }
    })
}

export const getMessagesAPI = () => {
    return httpInstance({
        url: '/messages'
    })
}
