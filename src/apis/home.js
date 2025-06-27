import httpInstance from '@/utils/http'

// 获取 banner

export function getBannerAPI() {
    return httpInstance({
        url: '/home/banner'
    })
}